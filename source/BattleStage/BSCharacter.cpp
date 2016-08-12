// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "BattleStage.h"
#include "BSCharacter.h"
#include "Weapons/BSProjectile.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"
#include "MotionControllerComponent.h"
#include "Weapons/BSWeapon.h"
#include "UnrealNetwork.h"
#include "GameModes/BSGameMode.h"
#include "BSCharacterMovementComponent.h"
#include "BSPlayerController.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

ABSCharacter::ABSCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBSCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// First person camera
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->RelativeLocation = FVector(0, 2.56f, 74.f);
	FirstPersonCamera->bUsePawnControlRotation = true;
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());

	// Setup first person mesh.
	// Only owner can see and does not replicate.
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetupAttachment(FirstPersonCamera);
	FirstPersonMesh->SetCastShadow(true);
	FirstPersonMesh->bSelfShadowOnly = true;
	FirstPersonMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	FirstPersonMesh->bOnlyOwnerSee = true;
	FirstPersonMesh->bReceivesDecals = false;
	FirstPersonMesh->RelativeRotation = FRotator{ 0.f, -90.f, -5.f };
	FirstPersonMesh->RelativeLocation = FVector{ 0, -5.f, -130.f };

	// Setup character mesh.
	// Owner can not see and does replicate.
	USkeletalMeshComponent* Mesh = GetMesh();
	Mesh->SetupAttachment(GetCapsuleComponent());
	Mesh->bOwnerNoSee = true;
	Mesh->bReceivesDecals = false;
	Mesh->RelativeLocation = FVector{ 0.f, 0.f, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight() };
	Mesh->RelativeRotation = FRotator{ 0.f, -90.f, 0.f };

	bIsDying = false;
	bIsRunning = false;
	Health = 100;
	RunningMovementModifier = 1.5f;
	CrouchCameraSpeed = 500.f;
	
	WeaponEquippedSocket = TEXT("GripPoint");
}

void ABSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABSCharacter, Weapon);
	DOREPLIFETIME(ABSCharacter, bIsDying);
	DOREPLIFETIME_CONDITION(ABSCharacter, bIsRunning, COND_SkipOwner);
	DOREPLIFETIME(ABSCharacter, Health);
	DOREPLIFETIME(ABSCharacter, ReceiveHitInfo);
}

float ABSCharacter::GetAimSpread() const
{
	if (Weapon)
		return Weapon->GetCurrentSpread();

	return 0.f;
}

float ABSCharacter::GetMovementModifier() const
{
	float Modifier = 1.0f;

	if (bIsRunning)
	{
		Modifier = RunningMovementModifier;
	}

	return Modifier;
}

void ABSCharacter::StartFire()
{
	if (Weapon)
	{
		if (bIsRunning)
		{
			SetRunning(false);
		}

		Weapon->StartFire();
	}
}

void ABSCharacter::StopFire()
{
	if (Weapon)
	{
		Weapon->StopFire();
	}
}

bool ABSCharacter::IsRunning() const
{
	return bIsRunning;
}

bool ABSCharacter::CanRun() const
{
	return !GetMovementComponent()->IsFalling();
}

void ABSCharacter::SetRunning(bool bNewRunning)
{
	if (bIsRunning != bNewRunning &&
		(!bNewRunning || CanRun()))
	{
		bIsRunning = bNewRunning;

		if (bNewRunning)			
		{
			// No firing while running
			if (Weapon)
			{
				Weapon->StopFire();
			}

			// Stop crouching before running
			UnCrouch();
		}

		if (!HasAuthority())
		{
			ServerSetRunning(bNewRunning);
		}
	}
}

void ABSCharacter::ServerSetRunning_Implementation(bool bNewRunning)
{
	SetRunning(bNewRunning);
}

bool ABSCharacter::ServerSetRunning_Validate(bool bNewRunning)
{
	return true;
}

void ABSCharacter::ToggleRunning()
{
	SetRunning(!bIsRunning);
}

void ABSCharacter::ReloadWeapon()
{
	if (Weapon)
		Weapon->Reload();
}

USkeletalMeshComponent* ABSCharacter::GetActiveMesh() const
{
	return IsFirstPerson() ? FirstPersonMesh : GetThirdPersonMesh();
}

void ABSCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// If we are running, make sure our pending movement is forward. If not, stop running.
	if (bIsRunning && IsLocallyControlled())
	{
		const FVector Movement = GetLastMovementInputVector().GetSafeNormal2D();
		const FVector Forward = GetActorForwardVector().GetSafeNormal2D();
		
		// Tolerance is half 30 degrees in the forward direction		
		const float Tolerance = FMath::Cos(PI / 6.0f);

		if (FVector::DotProduct(Movement, Forward) < Tolerance)
		{
			SetRunning(false);
		}
	}

	UpdateViewTarget(DeltaSeconds);
}

void ABSCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		EquipWeapon();
	}
}

float ABSCharacter::PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate /*= 1.f*/, FName StartSectionName /*= NAME_None*/)
{
	float Duration = 0.f;

	if (AnimMontage)
	{
		USkeletalMeshComponent* Mesh = GetActiveMesh();
		UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
		Duration = AnimInstance->Montage_Play(AnimMontage, InPlayRate);

		if (Duration > 0 && StartSectionName != NAME_None)
		{
			AnimInstance->Montage_JumpToSection(StartSectionName, AnimMontage);
		}
	}

	return Duration;
}

void ABSCharacter::StopAnimMontage(class UAnimMontage* AnimMontage /*= NULL*/)
{
	USkeletalMeshComponent* ActiveMesh = GetActiveMesh();
	UAnimInstance * AnimInstance = (ActiveMesh) ? ActiveMesh->GetAnimInstance() : nullptr;
	UAnimMontage * MontageToStop = (AnimMontage) ? AnimMontage : GetCurrentMontage();
	bool bShouldStopMontage = AnimInstance && MontageToStop && !AnimInstance->Montage_GetIsStopped(MontageToStop);

	if (bShouldStopMontage)
	{
		AnimInstance->Montage_Stop(MontageToStop->BlendOut.GetBlendTime(), MontageToStop);
	}
}

void ABSCharacter::Crouch(bool bClientSimulation /*= false*/)
{
	SetRunning(false);

	Super::Crouch(bClientSimulation);
}

void ABSCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	// Save eye height so we can lerp to new location following start crouch.
	FVector NewCameraLocation = FirstPersonCamera->RelativeLocation;
	NewCameraLocation.Z += HalfHeightAdjust;

	LastEyeHeight = NewCameraLocation.Z;
	FirstPersonCamera->SetRelativeLocation(NewCameraLocation);
}

void ABSCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	// Save eye height so we can lerp to new location following start crouch.
	FVector NewCameraLocation = FirstPersonCamera->RelativeLocation;
	NewCameraLocation.Z -= HalfHeightAdjust;

	LastEyeHeight = NewCameraLocation.Z;
	FirstPersonCamera->SetRelativeLocation(NewCameraLocation);
}

void ABSCharacter::Jump()
{
	SetRunning(false);

	Super::Jump();
}

bool ABSCharacter::ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	return CanDie() && Super::ShouldTakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

void ABSCharacter::PostInitProperties()
{
	Super::PostInitProperties();

	LastEyeHeight = BaseEyeHeight;
}

float ABSCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	TakeHit(ActualDamage, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage > 0)
	{
		Health -= ActualDamage;

		if (Health <= 0)
		{
			Die(DamageEvent, EventInstigator);
		}
	}

	return ActualDamage;
}

bool ABSCharacter::CanDie() const
{
	return Health > 0 && !IsPendingKill();
}

void ABSCharacter::Die(FDamageEvent const& DamageEvent, AController* Killer)
{
	bIsDying = true;
	bReplicateMovement = false;
	bTearOff = true;

	ABSGameMode* GameMode = Cast<ABSGameMode>(GetWorld()->GetAuthGameMode());
	GameMode->ScoreKill(Killer, GetController());

	// #bstodo Must detach equipped weapon and destroy loadout

	// Detach controller, character will be destroyed soon
	DetachFromControllerPendingDestroy();

	if (GetNetMode() != NM_DedicatedServer)
	{
		UpdateMeshVisibility();

		OnDeath();
	}
}

void ABSCharacter::OnRep_IsDying()
{
	UpdateMeshVisibility();

	OnDeath();
}

void ABSCharacter::TakeHit(const float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	SetRecieveHitInfo(Damage, DamageEvent, DamageCauser);

	ApplyDamageMomentum(Damage, DamageEvent, nullptr, DamageCauser);
	
	if(GetNetMode() != NM_DedicatedServer)
		OnRecieveHit();

	if (DamageCauser)
	{
		// Notify received damage on controller
		if (ABSPlayerController* DamagedController = Cast<ABSPlayerController>(GetController()))
		{
			DamagedController->NotifyRecievedDamage(DamageCauser->GetActorLocation());
		}

		// Notify hit if for controller that caused damage		
		if (ABSPlayerController* InstigatorController = Cast<ABSPlayerController>(EventInstigator))
		{
			InstigatorController->NotifyWeaponHit();
		}
	}
}

void ABSCharacter::UpdateViewTarget(const float DeltaSeconds)
{
	if (FMath::Abs(LastEyeHeight - BaseEyeHeight) > 0.01f)
	{
		// Get lerp alpha for camera from LastEyHeight to BaseEyeHeight
		const float Distance = BaseEyeHeight - LastEyeHeight;
		const float DeltaAlpha = CrouchCameraSpeed / FMath::Abs(Distance) * DeltaSeconds;

		// Get new camera height
		FVector Location = FirstPersonCamera->RelativeLocation;
		Location.Z = FMath::Lerp(LastEyeHeight, BaseEyeHeight, FMath::Min(DeltaAlpha, 1.0f));

		FirstPersonCamera->SetRelativeLocation(Location);
		LastEyeHeight = Location.Z;
	}
}

void ABSCharacter::SetRecieveHitInfo(const float Damage, FDamageEvent const& DamageEvent, AActor* DamageCauser)
{
	// Set direction based on damage event type
	//if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	//{
	//	const FPointDamageEvent& PointDamageEvent = static_cast<const FPointDamageEvent&>(DamageEvent);
	//	ReceiveHitInfo.Location = PointDamageEvent.ShotDirection;
	//}
	//else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	//{
	//	const FRadialDamageEvent& RadialDamageEvent = static_cast<const FRadialDamageEvent&>(DamageEvent);
	//	ReceiveHitInfo.Location = (RadialDamageEvent.Origin - GetActorLocation()).GetSafeNormal();
	//}

	ReceiveHitInfo.DamageCauser = DamageCauser;
	ReceiveHitInfo.Damage = Damage;
}

void ABSCharacter::OnRecieveHit_Implementation()
{

}

void ABSCharacter::OnDeath_Implementation()
{
	// Stop any existing montages
	StopAnimMontage();

	// Set collision properties
	static FName RagdollProfile{ TEXT("Ragdoll") };
	GetMesh()->SetCollisionProfileName(RagdollProfile);

	SetActorEnableCollision(true);

	static FName NoCollisionProfile{ TEXT("NoCollision") };
	GetCapsuleComponent()->SetCollisionProfileName(NoCollisionProfile);

	// Play optional death anim and then active ragdoll
	if (DeathAnim)
	{
		const float DeathAnimLength = PlayAnimMontage(DeathAnim);
		
		// Activate ragdoll after death anim
		FOnMontageBlendingOutStarted RagdollStart;
		RagdollStart.BindUObject(this, &ABSCharacter::OnDeathAnimEnded);
		GetMesh()->GetAnimInstance()->Montage_SetBlendingOutDelegate(RagdollStart);
	}
	else
	{
		EnableRagdollPhysics();
	}	
}

void ABSCharacter::OnDeathAnimEnded(UAnimMontage*, bool)
{
	EnableRagdollPhysics();
}

void ABSCharacter::EnableRagdollPhysics()
{
	if (GetMesh()->GetPhysicsAsset())
	{
		// Set all bodies of the mesh component to simulate
		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->WakeAllRigidBodies();

		GetMesh()->bBlendPhysics = true;
	}

	// Stop and disable any character movement
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);

	SetLifeSpan(10.0f);
}

bool ABSCharacter::IsFirstPerson() const
{
	return !bIsDying && IsLocallyControlled();
}

void ABSCharacter::UpdateMeshVisibility()
{
	const bool bFirstPerson = IsFirstPerson();

	GetMesh()->SetOwnerNoSee(!bFirstPerson);
	FirstPersonMesh->SetVisibility(bFirstPerson);
}

void ABSCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	SetOwner(NewController);

	if (Weapon)
		Weapon->AttachToOwner();
}

void ABSCharacter::EquipWeapon()
{
	if (DefaultWeaponClass)
	{
		if (!HasAuthority())
		{
			ServerEquipWeapon();
		}
		else
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Instigator = this;
			SpawnParams.Owner = this;
			Weapon = GetWorld()->SpawnActor<ABSWeapon>(DefaultWeaponClass, SpawnParams);			
			Weapon->ServerEquip(this);
		}
	}
}

void ABSCharacter::ServerEquipWeapon_Implementation()
{
	EquipWeapon();
}

bool ABSCharacter::ServerEquipWeapon_Validate()
{
	return true;
}

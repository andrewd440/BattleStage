// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "BattleStage.h"
#include "BSCharacter.h"
#include "Weapons/BSProjectile.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"
#include "MotionControllerComponent.h"
#include "Weapons/BSWeapon.h"
#include "UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

ABSCharacter::ABSCharacter()
	: Super()
	, WeaponEquippedSocket(TEXT("GripPoint"))
{
	// First person camera
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->RelativeLocation = FVector(2.57f, -4.72f, 72.54f);
	FirstPersonCamera->bUsePawnControlRotation = true;
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());

	// Setup first person mesh.
	// Only owner can see and does not replicate.
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetupAttachment(FirstPersonCamera);
	FirstPersonMesh->SetCastShadow(false);
	FirstPersonMesh->bOnlyOwnerSee = true;
	FirstPersonMesh->bReceivesDecals = false;
	FirstPersonMesh->RelativeRotation = FRotator{ -3.3f, -107.75f, -4.88f };
	FirstPersonMesh->RelativeLocation = FVector{ 10.22f, 22.63f, -157.02f };

	// Setup character mesh.
	// Owner can not see and does replicate.
	USkeletalMeshComponent* Mesh = GetMesh();
	Mesh->SetupAttachment(GetCapsuleComponent());
	Mesh->bOwnerNoSee = true;
	Mesh->bReceivesDecals = false;
	Mesh->RelativeLocation = FVector{ 0.f, 0.f, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight() };
	Mesh->RelativeRotation = FRotator{ 0.f, -90.f, 0.f };
}

void ABSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABSCharacter, Weapon);
	DOREPLIFETIME(ABSCharacter, bIsDying);
}

float ABSCharacter::GetAimSpread() const
{
	if (Weapon)
		return Weapon->GetCurrentSpread();

	return 0.f;
}

void ABSCharacter::StartFire()
{
	if (Weapon)
	{
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

bool ABSCharacter::ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	return CanDie() && Super::ShouldTakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
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
			Die(DamageEvent);
		}
	}

	return ActualDamage;
}

bool ABSCharacter::CanDie() const
{
	return Health > 0 && !IsPendingKill();
}

void ABSCharacter::Die(FDamageEvent const& DamageEvent)
{
	bIsDying = true;
	bReplicateMovement = false;
	bTearOff = true;

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
	SetRecieveHitInfo(Damage, DamageEvent);

	ApplyDamageMomentum(Damage, DamageEvent, nullptr, DamageCauser);
	
	OnRecieveHit();
}

void ABSCharacter::SetRecieveHitInfo(const float Damage, FDamageEvent const& DamageEvent)
{
	// Set direction based on damage event type
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent& PointDamageEvent = static_cast<const FPointDamageEvent&>(DamageEvent);
		ReceiveHitInfo.Direction = PointDamageEvent.ShotDirection;
	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		const FRadialDamageEvent& RadialDamageEvent = static_cast<const FRadialDamageEvent&>(DamageEvent);
		ReceiveHitInfo.Direction = (RadialDamageEvent.Origin - GetActorLocation()).GetSafeNormal();
	}

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

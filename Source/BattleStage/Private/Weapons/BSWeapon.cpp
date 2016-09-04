// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"

#include "Engine/ActorChannel.h"

#include "BSCharacter.h"
#include "BSNetworkUtils.h"
#include "BSShotType.h"
#include "BSWeapon.h"
#include "BSPlayerController.h"

ABSWeapon::ABSWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MuzzleSocket(TEXT("MuzzleAttach"))
	, ShotTypeClass(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bCanBeDamaged = false;
	bNetUseOwnerRelevancy = true;

	// First person weapon mesh
	MeshFP = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshFP"));
	MeshFP->SetCollisionProfileName("CharacterMesh");
	MeshFP->SetCastShadow(true);
	MeshFP->bSelfShadowOnly = true;
	MeshFP->bDisableClothSimulation = true;
	MeshFP->bReceivesDecals = false;
	MeshFP->bOnlyOwnerSee = true;
	RootComponent = MeshFP;

	// Third person weapon mesh
	MeshTP = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshTP"));
	MeshTP->SetCollisionProfileName("CharacterMesh");
	MeshTP->SetCastShadow(true);
	MeshTP->bReceivesDecals = false;
	MeshTP->bDisableClothSimulation = true;
	MeshTP->bOwnerNoSee = true;

	// Default weapon fire data
	WeaponStats.MaxAmmo = 120;
	WeaponStats.ClipSize = 30;
	WeaponStats.BaseDamage = 5.f;
	WeaponStats.FireRate = 0.1f;
	WeaponStats.Stability = 1.f;
	WeaponStats.BaseSpread = 2.f;
	WeaponStats.StandingSpreadIncrement = 1.5f;
	WeaponStats.MovingSpreadIncrement = 2.f;
	WeaponStats.RecoilSpreadIncrement = 0.5f;
	WeaponStats.RecoilPush = FVector2D{ 0, 1 };
	WeaponStats.RecoilPushSpread = 10.f;	
	WeaponStats.ReloadSpeed = 2.f;
	WeaponStats.bIsAuto = true;
}

void ABSWeapon::PostInitProperties()
{
	Super::PostInitProperties();

	RemainingClip = FMath::Min(WeaponStats.ClipSize, WeaponStats.MaxAmmo);
	RemainingAmmo = FMath::Max(WeaponStats.MaxAmmo - RemainingClip, 0);
}

bool ABSWeapon::ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	
	if (ShotType)
	{
		bWroteSomething |= Channel->ReplicateSubobject(ShotType, *Bunch, *RepFlags);
	}

	return bWroteSomething;
}

void ABSWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (HasAuthority() && ShotTypeClass)
	{
		ShotType = NewObject<UBSShotType>(this, ShotTypeClass, TEXT("ShotType"));
	}
}

void ABSWeapon::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);

	BSCharacter = Cast<ABSCharacter>(NewOwner);
	AttachToOwner();
}

void ABSWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABSWeapon, bServerFired, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ABSWeapon, WeaponState, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ABSWeapon, ShotType, COND_InitialOnly);
}

// Called when the game starts or when spawned
void ABSWeapon::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void ABSWeapon::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (CurrentRecoilSpread > 0.f)
	{
		// Reduce recoil spread
		CurrentRecoilSpread = FMath::Max(0.f, CurrentRecoilSpread - WeaponStats.Stability * DeltaTime);
	}
	
	if (CurrentRecoilOffset.GetAbsMax() > DELTA)
	{
		// Lerp recoil offset to zero
		const FVector2D NewRecoilOffset = FMath::Lerp(CurrentRecoilOffset, FVector2D::ZeroVector, DeltaTime * WeaponStats.Stability);

		BSCharacter->AddControllerPitchInput(CurrentRecoilOffset.Y - NewRecoilOffset.Y);
		BSCharacter->AddControllerYawInput(NewRecoilOffset.X - CurrentRecoilOffset.X);

		CurrentRecoilOffset = NewRecoilOffset;
	}
}

void ABSWeapon::AttachToOwner()
{
	// Rid of current attachments
	DetachFromOwner();

	if (BSCharacter)
	{
		const FName AttachSocket = BSCharacter->GetWeaponEquippedSocket();

		USkeletalMeshComponent* const ActiveMesh = GetActiveMesh();
		ActiveMesh->AttachToComponent(BSCharacter->GetActiveMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachSocket);
		ActiveMesh->SetHiddenInGame(false);
	}
	else
	{
		// Only shot MeshTP there is no character owner
		MeshTP->SetHiddenInGame(false);
	}
}

void ABSWeapon::DetachFromOwner()
{
	MeshFP->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	MeshFP->SetHiddenInGame(true);

	MeshTP->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	MeshTP->SetHiddenInGame(true);
}

void ABSWeapon::Equip()
{
	AttachToOwner();

	SetWeaponState(EWeaponState::Equipping);
}

void ABSWeapon::Unequip()
{
	SetWeaponState(EWeaponState::Unequipping);
}

void ABSWeapon::StartFire()
{
	SetWeaponState(EWeaponState::Firing);
}

void ABSWeapon::StopFire()
{
	if(WeaponState != EWeaponState::Reloading)
		SetWeaponState(EWeaponState::Active);
}

float ABSWeapon::GetCurrentSpread() const
{
	// Get spread factor based on movement
	const float MovementFactor = BSCharacter->GetVelocity().Size() / BSCharacter->GetMovementComponent()->GetMaxSpeed();
	const float MovementSpread = MovementFactor * WeaponStats.MovingSpreadIncrement;

	// Get spread factor based on crouch/standing
	const float StandingSpread = BSCharacter->bIsCrouched ? 0.f : WeaponStats.StandingSpreadIncrement;

	return WeaponStats.BaseSpread + MovementSpread + StandingSpread + CurrentRecoilSpread;
}

UAnimMontage* ABSWeapon::GetWeaponMontage(const FWeaponAnim& WeaponAnim)
{
	return (BSCharacter->IsFirstPerson()) ? WeaponAnim.FirstPerson : WeaponAnim.ThirdPerson;
}

void ABSWeapon::PlayFiringSequence()
{
	const bool bMustSpawnFX = (!MuzzleFX->IsLooping() || !MuzzleFXComponent);
	if (MuzzleFX && bMustSpawnFX)
	{
		if (bMustSpawnFX)
		{
			MuzzleFXComponent = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, GetActiveMesh(), MuzzleSocket);
		}			

		MuzzleFXComponent->Activate(true);
	}

	const bool bMustSpawnSound = (!FireSound->IsLooping() || !FireSoundComponent);
	if (FireSound && bMustSpawnSound)
	{
		if (bMustSpawnSound)
		{
			FireSoundComponent = UGameplayStatics::SpawnSoundAttached(FireSound, GetActiveMesh(), MuzzleSocket);
		}			
		
		FireSoundComponent->Play();
	}

	if (UAnimMontage* FireMontage = GetWeaponMontage(FireAnim))
	{
		BSCharacter->PlayAnimMontage(FireMontage);
	}

	BSCharacter->AddControllerPitchInput(-.1f);

	if (FireCameraShake &&
		BSCharacter->IsFirstPerson() &&
		BSCharacter->IsLocallyControlled())
	{
		APlayerController* const PlayerController = static_cast<APlayerController* const>(BSCharacter->GetController());
		PlayerController->ClientPlayCameraShake(FireCameraShake);
	}
}

void ABSWeapon::SetWeaponState(EWeaponState NewState)
{
	if (WeaponState != NewState)
	{
		switch (NewState)
		{
		case EWeaponState::Firing:
			if (!CanFire())
			{
				if (CanReload())
					NewState = EWeaponState::Reloading;
				else
				{
					NewState = EWeaponState::Active;
					PlayEmptyClipSequence();
				}
			}
			break;
		case EWeaponState::Reloading:
			if (!CanReload())
				NewState = EWeaponState::Active;
			break;
		case EWeaponState::Inactive:
			if(WeaponState != EWeaponState::Unequipping)
				UE_LOG(BattleStage, Warning, TEXT("ABSWeapon setting to inactive before unequipping."));
			break;
		case EWeaponState::Equipping:
			if(WeaponState != EWeaponState::Inactive)
				UE_LOG(BattleStage, Warning, TEXT("ABSWeapon setting to equipping while not inactive."));
			break;
		}

		// Make sure the weapon state is really changing
		if (WeaponState != NewState)
		{
			// Set state locally
			WeaponState = NewState;
			OnNewWeaponState();

			if (!HasAuthority())
			{
				// Make sure the transition is sent to the server
				ServerSetWeaponState(NewState);
			}			
		}			
	}
}

void ABSWeapon::ServerSetWeaponState_Implementation(const EWeaponState NewState)
{
	WeaponState = NewState;

	OnNewWeaponState();
}

bool ABSWeapon::ServerSetWeaponState_Validate(const EWeaponState State)
{
	return true;
}

void ABSWeapon::OnNewWeaponState()
{
	if (PrevWeaponState == EWeaponState::Firing)
		OnExitFiringState();

	switch (WeaponState)
	{
	case EWeaponState::Inactive:
		OnEnteredInactiveState();
		break;
	case EWeaponState::Equipping:
		OnEquipTransitionStart();
		break;
	case EWeaponState::Active:
		OnEnteredActiveState();
		break;
	case EWeaponState::Unequipping:
		OnUnequipTransitionStart();
		break;
	case EWeaponState::Firing:
		OnEnteredFiringState();
		break;
	case EWeaponState::Reloading:
		OnReloadTransitionStart();
		break;
	default:
		break;
	}

	PrevWeaponState = WeaponState;
}

void ABSWeapon::OnEquipTransitionStart()
{
	OnEnteredEquippingState();

	float TransitionTime = 0.f;
	if (UAnimMontage* const Montage = GetWeaponMontage(EquipAnim))
	{
		TransitionTime = Montage->GetPlayLength();
	}

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(WeaponStateTimer);
	
	if (TransitionTime >= 0.f)
	{
		TimerManager.SetTimer(WeaponStateTimer, this, &ABSWeapon::OnEquipTransitionExit, TransitionTime);
	}
	else
	{
		OnEquipTransitionExit();
	}
}

void ABSWeapon::OnEquipTransitionExit()
{
	SetWeaponState(EWeaponState::Active);
}

void ABSWeapon::OnReloadTransitionStart()
{
	OnEnteredReloadingState();

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(WeaponStateTimer);
	
	if (WeaponStats.ReloadSpeed >= 0.f)
	{
		TimerManager.SetTimer(WeaponStateTimer, this, &ABSWeapon::OnReloadTransitionExit, WeaponStats.ReloadSpeed);
	}
	else
	{
		OnReloadTransitionExit();
	}
}

void ABSWeapon::OnReloadTransitionExit()
{
	// Update clip and ammo
	const int32 EmptySlots = WeaponStats.ClipSize - RemainingClip;
	const int32 RefillCount = FMath::Min(RemainingAmmo, EmptySlots);
	RemainingClip += RefillCount;
	RemainingAmmo -= RefillCount;

	SetWeaponState(EWeaponState::Active);
}

void ABSWeapon::OnUnequipTransitionStart()
{
	OnEnteredUnequippingState();

	float TransitionTime = 0.f;
	if (UAnimMontage* const Montage = GetWeaponMontage(UnequipAnim))
	{
		TransitionTime = Montage->GetPlayLength();
	}

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(WeaponStateTimer);

	if (TransitionTime >= 0.f)
	{
		TimerManager.SetTimer(WeaponStateTimer, this, &ABSWeapon::OnUnequipTransitionExit, TransitionTime);
	}
	else
	{
		OnUnequipTransitionExit();
	}	
}

void ABSWeapon::OnUnequipTransitionExit()
{
	SetWeaponState(EWeaponState::Inactive);
}

void ABSWeapon::FireShot()
{
	if (CanFire())
	{
		if (!ShotType)
		{
			UE_LOG(BattleStage, Warning, TEXT("ABSWeapon trying to fire without a valid ShotType"));
		}
		else
		{
			FShotData ShotData;
			if (ShotType->GetShotData(ShotData))
			{
				ShotType->PreInvokeShot(ShotData);

				InvokeShot(ShotData);				

				LastFireTime = GetWorld()->GetTimeSeconds();
			}
		}
	}
	else if (CanReload())
	{
		SetWeaponState(EWeaponState::Reloading);
	}
	else
	{
		SetWeaponState(EWeaponState::Active);
		PlayEmptyClipSequence();
	}
}

void ABSWeapon::InvokeShot(const FShotData& ShotData)
{
	if (!HasAuthority())
	{
		ServerInvokeShot(ShotData);
		OnShotFired();
		--RemainingClip;
	}
	else
	{
		// Ensure the weapon can fire on the server. If not, force a state change.
		if (CanFire())
		{
			ShotType->InvokeShot(ShotData);

			--RemainingClip;

			bServerFired = !bServerFired;

			if (GetNetMode() != NM_DedicatedServer)
				OnRep_ServerFired();
		}
	}
}

void ABSWeapon::OnShotFired()
{
	PlayFiringSequence();

	CurrentRecoilSpread += WeaponStats.RecoilSpreadIncrement;

	if (BSCharacter->IsLocallyControlled())
	{
		// Only apply recoil to player if locally controlled. Replicated
		// view direction will allow this to replicate to all clients.		
		const int32 Seed = FMath::Rand();
		FRandomStream RecoilOffsetStream(Seed);

		// Get random rotation [-1, 1] to factor in with RecoilPushSpread and
		// apply the rotation to RecoilPush
		const float RSeed = 2.f * FMath::FRand() - 1.f;
		const float Rotation = RSeed * WeaponStats.RecoilPushSpread;
		const FVector2D RecoilInput = WeaponStats.RecoilPush.GetRotated(Rotation);

		BSCharacter->AddControllerPitchInput(-RecoilInput.Y);
		BSCharacter->AddControllerYawInput(RecoilInput.X);

		CurrentRecoilOffset += RecoilInput;
	}
}

void ABSWeapon::OnEnteredFiringState()
{
	if (BSCharacter->IsLocallyControlled())
	{
		// Use delay to prevent tap firing faster than the fire rate of the weapon.
		const float CurrentTime = GetWorld()->GetTimeSeconds();
		const float ShotDelay = FMath::Max(0.f, WeaponStats.FireRate - (CurrentTime - LastFireTime));

		GetWorldTimerManager().SetTimer(WeaponFiringTimer, this, &ABSWeapon::FireShot, WeaponStats.FireRate, WeaponStats.bIsAuto, ShotDelay);
	}
}

void ABSWeapon::OnExitFiringState()
{
	if (BSCharacter->IsLocallyControlled())
	{
		GetWorldTimerManager().ClearTimer(WeaponFiringTimer);
	}

	if (MuzzleFXComponent && MuzzleFX->IsLooping())
	{
		MuzzleFXComponent->DeactivateSystem();
		MuzzleFXComponent = nullptr;
	}

	if (FireSoundComponent && FireSound->IsLooping())
	{
		FireSoundComponent->FadeOut(0.1f, 0.0f);
		FireSoundComponent = nullptr;
	}

	if (EndFireSound)
	{
		UGameplayStatics::SpawnSoundAttached(EndFireSound, RootComponent);
	}
}

void ABSWeapon::OnEnteredEquippingState()
{
	PlayEquipSequence();
}

void ABSWeapon::OnEnteredActiveState()
{
	// Do nothing... for now
}

void ABSWeapon::OnEnteredUnequippingState()
{
	PlayUnequipSequence();
}

void ABSWeapon::OnEnteredReloadingState()
{
	PlayReloadSequence();
}

void ABSWeapon::OnEnteredInactiveState()
{
	// Do nothing... for now
}

void ABSWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();

	BSCharacter = Cast<ABSCharacter>(GetOwner());
	AttachToOwner();
}

void ABSWeapon::ServerInvokeShot_Implementation(const FShotData& ShotData)
{
	InvokeShot(ShotData);
}

bool ABSWeapon::ServerInvokeShot_Validate(const FShotData& ShotData)
{
	return true;
}

void ABSWeapon::PlayEmptyClipSequence()
{
	if (EmptyClipSound)
	{
		UGameplayStatics::SpawnSoundAttached(EmptyClipSound, RootComponent);
	}
}

bool ABSWeapon::CanFire() const
{
	if (WeaponState == EWeaponState::Firing || WeaponState == EWeaponState::Active)
	{
		return RemainingClip > 0;
	}

	return false;
}

bool ABSWeapon::CanReload() const
{
	if (WeaponState == EWeaponState::Firing || WeaponState == EWeaponState::Active || WeaponState == EWeaponState::Reloading)
	{
		return RemainingClip < WeaponStats.ClipSize && RemainingAmmo > 0;
	}

	return false;
}

void ABSWeapon::Reload()
{
	if (CanReload())
		SetWeaponState(EWeaponState::Reloading);
}

void ABSWeapon::OnRep_ServerFired()
{
	// Make sure we are still firing to prevent incorrect behavior
	// from replication order when ServerFired and WeaponState are changed
	// at the same time.
	if (WeaponState == EWeaponState::Firing)
		OnShotFired();
}

void ABSWeapon::OnRep_WeaponState()
{
	if (PrevWeaponState == EWeaponState::Firing)
		OnExitFiringState();

	switch (WeaponState)
	{
	case EWeaponState::Inactive:
		OnEnteredInactiveState();
		break;
	case EWeaponState::Equipping:
		OnEnteredEquippingState();
		break;
	case EWeaponState::Active:
		OnEnteredActiveState();
		break;
	case EWeaponState::Unequipping:
		OnEnteredUnequippingState();
		break;
	case EWeaponState::Firing:
		OnEnteredFiringState();
		break;
	case EWeaponState::Reloading:
		OnEnteredReloadingState();
		break;
	default:
		break;
	}

	PrevWeaponState = WeaponState;
}

FRotator ABSWeapon::GetFireRotation_Implementation() const
{
	return GetActiveMesh()->GetSocketRotation(MuzzleSocket);
}

FVector ABSWeapon::GetFireLocation_Implementation() const
{
	return GetActiveMesh()->GetSocketLocation(MuzzleSocket);
}

FRotator ABSWeapon::GetAimRotation() const
{
	return BSCharacter->GetBaseAimRotation();
}

FVector ABSWeapon::GetAimLocation() const
{
	FVector AimLocation(ForceInitToZero);

	if (BSCharacter->IsFirstPerson())
	{	
		APlayerController* PlayerController = static_cast<APlayerController*>(BSCharacter->GetController());
		
		FRotator DisgardRotator{};
		PlayerController->GetPlayerViewPoint(AimLocation, DisgardRotator);
		
		// Move the aim location to the distance of the muzzle from the players view.
		const FVector MuzzleOffset = GetFireLocation() - AimLocation;
		const FVector AimDirection = GetFireRotation().Vector();
		AimLocation += FVector::DotProduct(MuzzleOffset, AimDirection) * AimDirection;
	}
	else
	{
		AimLocation = GetFireLocation();
	}

	return AimLocation;
}

float ABSWeapon::PlayEquipSequence()
{
	float SequenceLength = 0.f;

	if (UAnimMontage* EquipMontage = GetWeaponMontage(EquipAnim))
	{
		SequenceLength = BSCharacter->PlayAnimMontage(EquipMontage);
	}

	return SequenceLength;
}

float ABSWeapon::PlayUnequipSequence()
{
	float SequenceLength = 0.f;

	if (UAnimMontage* UnequipMontage = GetWeaponMontage(UnequipAnim))
	{
		SequenceLength = BSCharacter->PlayAnimMontage(UnequipMontage);
	}

	return SequenceLength;
}

float ABSWeapon::PlayReloadSequence()
{
	if (UAnimMontage* ReloadMontage = GetWeaponMontage(ReloadAnim))
	{
		const float AnimLengthScale = ReloadMontage->CalculateSequenceLength() / WeaponStats.ReloadSpeed;
		const float AnimLength = BSCharacter->PlayAnimMontage(ReloadMontage, AnimLengthScale);
	}

	return WeaponStats.ReloadSpeed;
}

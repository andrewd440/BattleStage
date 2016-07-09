// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"

#include "UnrealNetwork.h"

#include "BSCharacter.h"
#include "BSNetworkUtils.h"
#include "Weapons/BSShotType.h"
#include "Weapons/BSWeapon.h"
#include "Engine/ActorChannel.h"

ABSWeapon::ABSWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MuzzleSocket(TEXT("MuzzleAttach"))
	, ShotTypeClass(nullptr)
{
	MeshFP = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshFP"));
	MeshFP->SetCollisionProfileName("CharacterMesh");
	MeshFP->SetCastShadow(false);
	MeshFP->bDisableClothSimulation = true;
	MeshFP->bReceivesDecals = false;
	MeshFP->bOnlyOwnerSee = true;
	RootComponent = MeshFP;


	MeshTP = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshTP"));
	MeshTP->SetCollisionProfileName("CharacterMesh");
	MeshTP->SetCastShadow(true);
	MeshTP->bReceivesDecals = false;
	MeshTP->bDisableClothSimulation = true;
	MeshTP->bOwnerNoSee = true;
	MeshTP->SetupAttachment(RootComponent);

	bReplicates = true;
	bCanBeDamaged = false;

	// Default weapon fire data
	WeaponStats.MaxAmmo = 120;
	WeaponStats.ClipSize = 30;
	WeaponStats.BaseDamage = 5.0f;
	WeaponStats.FireRate = 0.1f;
	WeaponStats.BaseSpread = 2.0f;
	WeaponStats.SpreadIncrementStanding = 1.0f;
	WeaponStats.SpreadIncrementMoving = 1.0f;
	WeaponStats.ReloadSpeed = 2.0f;
	WeaponStats.bIsAuto = true;
}

void ABSWeapon::PostInitProperties()
{
	Super::PostInitProperties();

	RemainingClip = WeaponStats.ClipSize;
	RemainingAmmo = WeaponStats.MaxAmmo - RemainingClip;
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

void ABSWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABSWeapon, bServerFired, COND_SkipOwner);
	DOREPLIFETIME(ABSWeapon, BSCharacter);
	DOREPLIFETIME(ABSWeapon, WeaponState); // Remotes need to play animations, etc.
	DOREPLIFETIME_CONDITION(ABSWeapon, RemainingAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ABSWeapon, RemainingClip, COND_OwnerOnly);
	DOREPLIFETIME(ABSWeapon, ShotType);
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
}

void ABSWeapon::ServerEquip_Implementation(ABSCharacter* Character)
{
	BSCharacter = Character;

	OnRep_BSCharacter();

	SetWeaponState(EWeaponState::Equipping);
}

bool ABSWeapon::ServerEquip_Validate(ABSCharacter* Character)
{
	return true;
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
	const float MovementSpread = MovementFactor * WeaponStats.SpreadIncrementMoving;

	// Get spread factor based on crouch/standing
	const float StandingSpread = BSCharacter->bIsCrouched ? 0.f : WeaponStats.SpreadIncrementStanding;

	return WeaponStats.BaseSpread + MovementSpread + StandingSpread;
}

void ABSWeapon::PlayFireEffects()
{
	if (MuzzleFX && (!MuzzleFX->IsLooping() || !MuzzleFXComponent))
	{
		if (!MuzzleFXComponent)
		{
			MuzzleFXComponent = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, GetActiveMesh(), MuzzleSocket);
		}			

		MuzzleFXComponent->Activate(true);
	}

	if (FireSound && (!FireSound->IsLooping() || !FireSoundComponent))
	{
		if(!FireSoundComponent)
			FireSoundComponent = UGameplayStatics::SpawnSoundAttached(FireSound, GetActiveMesh(), MuzzleSocket);
		
		FireSoundComponent->Play();
	}

	if (FireAnim)
		BSCharacter->PlayAnimMontage(FireAnim);
}

void ABSWeapon::SetWeaponState(EWeaponState State)
{
	if (WeaponState != State)
	{
		switch (State)
		{
		case EWeaponState::Firing:
			if (!CanFire())
			{
				if (CanReload())
					State = EWeaponState::Reloading;
				else
					State = EWeaponState::Active;
			}
			break;
		case EWeaponState::Reloading:
			if (!CanReload())
				State = EWeaponState::Active;
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

		if(WeaponState != State)
			HandleNewWeaponState(State);
	}
}

void ABSWeapon::HandleNewWeaponState(const EWeaponState State)
{
	if (!HasAuthority())
	{
		ServerHandleNewWeaponState(State);
	}
	else
	{
		if (WeaponState == EWeaponState::Firing)
			ClientExitFiringState();

		WeaponState = State;

		FTimerManager& TimerManager = GetWorldTimerManager();
		TimerManager.ClearTimer(WeaponStateTimer);

		switch(State)
		{
		case EWeaponState::Equipping:
			{
				const float SequenceLength = PlayEquipSequence();
				if (SequenceLength > 0)
				{
					TimerManager.SetTimer(WeaponStateTimer, this, &ABSWeapon::OnEquipFinished, SequenceLength);
				}
				else if (SequenceLength == 0)
				{
					OnEquipFinished();
				}
				break;
			}
		case EWeaponState::Unequipping:
			{
				const float SequenceLength = PlayUnequipSequence();
				if (SequenceLength > 0)
				{
					TimerManager.SetTimer(WeaponStateTimer, this, &ABSWeapon::OnUnquipFinished, SequenceLength);
				}
				else if (SequenceLength == 0)
				{
					OnUnquipFinished();
				}
				break;
			}
		case EWeaponState::Reloading:			
			{
				const float SequenceLength = PlayReloadSequence();
				if (SequenceLength > 0)
				{
					TimerManager.SetTimer(WeaponStateTimer, this, &ABSWeapon::OnReloadFinished, SequenceLength);
				}
				else if (SequenceLength == 0)
				{
					OnReloadFinished();
				}
				break;
			}
		case EWeaponState::Firing:
			{
				ClientEnteredFiringState();
				break;				
			}
		}
		
		if (GetNetMode() != NM_DedicatedServer)
			OnRep_WeaponState();
	}
}

void ABSWeapon::ServerHandleNewWeaponState_Implementation(const EWeaponState State)
{
	HandleNewWeaponState(State);
}

bool ABSWeapon::ServerHandleNewWeaponState_Validate(const EWeaponState State)
{
	bool bIsValid = true;

	switch (State)
	{
	case EWeaponState::Firing:
		bIsValid = CanFire();
		break;
	case EWeaponState::Reloading:
		bIsValid = CanReload();
		break;
	}

	return bIsValid;
}

void ABSWeapon::ClientEnteredFiringState_Implementation()
{
	// Use delay to prevent tap firing faster than the fire rate of the weapon.
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const float ShotDelay = FMath::Max(0.f, WeaponStats.FireRate - (CurrentTime - LastFireTime));

	GetWorldTimerManager().SetTimer(WeaponFiringTimer, this, &ABSWeapon::FireShot, WeaponStats.FireRate, WeaponStats.bIsAuto, ShotDelay);
}

void ABSWeapon::ClientExitFiringState_Implementation()
{
	GetWorldTimerManager().ClearTimer(WeaponFiringTimer);
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

				// Invoke the actual shot on the server
				ServerInvokeShot(ShotData);

				if(GetNetMode() == NM_Client)
					PlayFireEffects();

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
	}
}

void ABSWeapon::ServerInvokeShot_Implementation(const FShotData& ShotData)
{
	// Ensure the weapon can fire on the server. If not, force a state change.
	if (CanFire())
	{
		ShotType->InvokeShot(ShotData);

		RemainingClip--;

		bServerFired = !bServerFired;

		if (GetNetMode() != NM_DedicatedServer)
			OnRep_ServerFired();
	}
	else if (CanReload())
	{
		SetWeaponState(EWeaponState::Reloading);
	}
	else
	{
		SetWeaponState(EWeaponState::Active);
	}
}

bool ABSWeapon::ServerInvokeShot_Validate(const FShotData& ShotData)
{
	return true;
}

void ABSWeapon::PlayBeginFireSequence()
{
	if (BeginFireSound)
	{
		UGameplayStatics::SpawnSoundAttached(BeginFireSound, RootComponent);
	}
}

void ABSWeapon::PlayEndFireSequence()
{
	if (MuzzleFXComponent)
	{
		MuzzleFXComponent->DeactivateSystem();
		MuzzleFXComponent = nullptr;
	}

	if (FireSoundComponent)
	{
		FireSoundComponent->FadeOut(0.1f, 0.0f);
		FireSoundComponent = nullptr;
	}

	if (EndFireSound)
	{
		UGameplayStatics::SpawnSoundAttached(EndFireSound, RootComponent);
	}
}

void ABSWeapon::OnEquipFinished()
{
	SetWeaponState(EWeaponState::Active);
}

void ABSWeapon::OnReloadFinished()
{
	const int32 EmptySlots = WeaponStats.ClipSize - RemainingClip;
	const int32 RefillCount = FMath::Min(RemainingAmmo, EmptySlots);
	RemainingClip += RefillCount;
	RemainingAmmo -= RefillCount;
	SetWeaponState(EWeaponState::Active);
}

void ABSWeapon::OnUnquipFinished()
{
	SetWeaponState(EWeaponState::Inactive);
	DetachRootComponentFromParent();

	BSCharacter = nullptr;
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
	if(WeaponState == EWeaponState::Firing)
		PlayFireEffects();
}

void ABSWeapon::OnRep_WeaponState()
{
	if (PrevWeaponState == EWeaponState::Firing)
		PlayEndFireSequence();

	switch (WeaponState)
	{
	case EWeaponState::Equipping:
		PlayEquipSequence();
		break;
	case EWeaponState::Unequipping:
		PlayUnequipSequence();
		break;
	case EWeaponState::Reloading:
		PlayReloadSequence();
		break;
	case EWeaponState::Firing:
		PlayBeginFireSequence();
		break;
	}

	PrevWeaponState = WeaponState;
}

void ABSWeapon::OnRep_BSCharacter()
{
	if (BSCharacter->IsFirstPerson())
	{
		const FAttachmentTransformRules AttachRules{ EAttachmentRule::SnapToTarget, true };
		const FName AttachSocket = BSCharacter->GetWeaponEquippedSocket();
		MeshFP->AttachToComponent(BSCharacter->GetFirstPersonMesh(), AttachRules, AttachSocket);
	}
	
	// Attach MeshTP. Controlling clients will attach MeshFP on rep.
	const FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	const FName AttachSocket = BSCharacter->GetWeaponEquippedSocket();
	MeshTP->AttachToComponent(BSCharacter->GetThirdPersonMesh(), AttachRules, AttachSocket);
	
}

FRotator ABSWeapon::GetFireRotation_Implementation() const
{
	return BSCharacter->GetBaseAimRotation();
}

FVector ABSWeapon::GetFireLocation_Implementation() const
{
	return GetActiveMesh()->GetSocketLocation(MuzzleSocket);
}

FVector ABSWeapon::GetCameraAimLocation() const
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

	if (EquipAnim)
	{
		SequenceLength = BSCharacter->PlayAnimMontage(EquipAnim);
	}

	return SequenceLength;
}

float ABSWeapon::PlayUnequipSequence()
{
	float SequenceLength = 0.f;

	if (UnequipAnim)
	{
		SequenceLength = BSCharacter->PlayAnimMontage(UnequipAnim);
	}

	return SequenceLength;
}

float ABSWeapon::PlayReloadSequence()
{
	if (ReloadAnim)
	{
		const float AnimLengthScale = ReloadAnim->CalculateSequenceLength() / WeaponStats.ReloadSpeed;
		const float AnimLength = BSCharacter->PlayAnimMontage(ReloadAnim, AnimLengthScale);
	}

	return WeaponStats.ReloadSpeed;
}

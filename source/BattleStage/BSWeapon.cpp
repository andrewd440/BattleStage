// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSWeapon.h"
#include "BSCharacter.h"
#include "UnrealNetwork.h"
#include "BSNetworkUtils.h"

ABSWeapon::ABSWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MuzzleSocket(TEXT("MuzzleAttach"))
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
	WeaponFireData.MaxAmmo = 120;
	WeaponFireData.ClipSize = 30;
	WeaponFireData.FireRate = 0.1f;
	WeaponFireData.ReloadSpeed = 2.0f;
	WeaponFireData.bIsAuto = true;
}

void ABSWeapon::PostInitProperties()
{
	Super::PostInitProperties();

	RemainingClip = WeaponFireData.ClipSize;
	RemainingAmmo = WeaponFireData.MaxAmmo - RemainingClip;
}

void ABSWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABSWeapon, bServerFired, COND_SkipOwner);
	DOREPLIFETIME(ABSWeapon, BSCharacter);
	DOREPLIFETIME(ABSWeapon, WeaponState); // Remotes need to play animations, etc.
	DOREPLIFETIME_CONDITION(ABSWeapon, RemainingAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ABSWeapon, RemainingClip, COND_OwnerOnly);
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

	// Attach MeshTP. Contolling clients will attach MeshFP on rep.
	const FAttachmentTransformRules AttachRules{ EAttachmentRule::SnapToTarget, true };
	const FName AttachSocket = BSCharacter->GetWeaponEquippedSocket();
	MeshTP->AttachToComponent(BSCharacter->GetThirdPersonMesh(), AttachRules, AttachSocket);

	if (GetNetMode() != NM_DedicatedServer)
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

void ABSWeapon::SpawnProjectile(const FVector& Position, const FVector_NetQuantizeNormal& Direction)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = BSCharacter;

	GetWorld()->SpawnActor<ABSProjectile>(ProjectileClass, Position, Direction.Rotation(), SpawnParams);
}

void ABSWeapon::PlayFireEffects()
{
	if (MuzzleFX)
	{
		if (MuzzleFXComponent)
		{
			MuzzleFXComponent->DeactivateSystem();
			MuzzleFXComponent->DestroyComponent();
		}

		MuzzleFXComponent = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, GetActiveMesh(), MuzzleSocket);	
	}

	if(FireSound)
		UGameplayStatics::SpawnSoundAttached(FireSound, GetActiveMesh(), MuzzleSocket);
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
	GetWorldTimerManager().SetTimer(WeaponFiringTimer, this, &ABSWeapon::FireShot, WeaponFireData.FireRate, WeaponFireData.bIsAuto, 0.f);
}

void ABSWeapon::ClientExitFiringState_Implementation()
{
	GetWorldTimerManager().ClearTimer(WeaponFiringTimer);
}

void ABSWeapon::FireShot()
{
	if (CanFire())
	{
		ServerFire(GetFireLocation(), GetFireRotation().Vector());
		PlayFireEffects();
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

void ABSWeapon::ServerFire_Implementation(FVector Position, FVector_NetQuantizeNormal Direction)
{
	if (RemainingClip > 0)
	{
		RemainingClip--;

		if (ProjectileClass)
			SpawnProjectile(Position, Direction);

		bServerFired = !bServerFired;

		if (GetNetMode() != NM_DedicatedServer)
			OnRep_ServerFired();
	}
	else if (RemainingAmmo > 0)
	{
		SetWeaponState(EWeaponState::Reloading);
	}
	else
	{
		SetWeaponState(EWeaponState::Active);
	}
}

bool ABSWeapon::ServerFire_Validate(FVector Position, FVector_NetQuantizeNormal Direction)
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
	const int32 EmptySlots = WeaponFireData.ClipSize - RemainingClip;
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
		return RemainingAmmo > 0;
	}

	return false;
}

void ABSWeapon::OnRep_ServerFired()
{
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
}

FRotator ABSWeapon::GetFireRotation_Implementation() const
{
	return BSCharacter->GetViewRotation();
}

FVector ABSWeapon::GetFireLocation_Implementation() const
{
	return GetActiveMesh()->GetSocketLocation(MuzzleSocket);
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
		const float AnimLengthScale = ReloadAnim->CalculateSequenceLength() / WeaponFireData.ReloadSpeed;
		const float AnimLength = BSCharacter->PlayAnimMontage(ReloadAnim, AnimLengthScale);
	}

	return WeaponFireData.ReloadSpeed;
}

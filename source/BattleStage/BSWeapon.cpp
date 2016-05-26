// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSWeapon.h"
#include "BSCharacter.h"
#include "UnrealNetwork.h"
#include "BSNetworkUtils.h"

FName ABSWeapon::FireEffectName(TEXT("FireEffect"));

// Sets default values
ABSWeapon::ABSWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CharacterOwner(nullptr)
	, FireOffset(EForceInit::ForceInitToZero)
	, SocketOffset(EForceInit::ForceInitToZero)
{
	//PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->bDisableClothSimulation = true;
	Mesh->SetCollisionProfileName("CharacterMesh");
	RootComponent = Mesh;

	MuzzleEffect = CreateOptionalDefaultSubobject<UParticleSystemComponent>(FireEffectName);
	MuzzleEffect->SetupAttachment(Mesh);
}

void ABSWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABSWeapon, bServerFired, COND_SkipOwner);
	DOREPLIFETIME(ABSWeapon, CharacterOwner);//, COND_SkipOwner);
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

void ABSWeapon::OnEquip(ABSCharacter* Character)
{
	FAttachmentTransformRules AttachRules{ EAttachmentRule::KeepRelative, true };
	AttachToComponent(Character->GetFirstPersonMesh(), AttachRules, FName{TEXT("GripPoint")});
	SetActorRelativeLocation(SocketOffset);

	CharacterOwner = Character;
}

void ABSWeapon::OnUnequip()
{
	DetachRootComponentFromParent();

	CharacterOwner = nullptr;
}

void ABSWeapon::Fire()
{
	if (CanFire() && (MuzzleEffect || ProjectileClass))
	{
		if (!HasAuthority())
		{
			ServerFire();
			PlayFireEffects();
		}
		else
		{
			if (ProjectileClass)
				SpawnProjectile();

			PlayFireEffects();
			bServerFired = !bServerFired;
		}
	}
}

void ABSWeapon::ServerFire_Implementation()
{
	Fire();
}

bool ABSWeapon::ServerFire_Validate()
{
	return true;
}

void ABSWeapon::BeginFireSequence()
{
	if (BeginFireSequenceSound)
	{
		if (!HasAuthority())
		{
			UGameplayStatics::SpawnSoundAttached(BeginFireSequenceSound, RootComponent);
			ServerBeginFireSequence();
		}
		else
		{
			UBSNetworkUtils::PlaySound(BeginFireSequenceSound, this, FVector::ZeroVector, EReplicationOption::AllButOWner);
		}
	}
}

void ABSWeapon::ServerBeginFireSequence_Implementation()
{
	BeginFireSequence();
}

bool ABSWeapon::ServerBeginFireSequence_Validate()
{
	return true;
}

void ABSWeapon::SpawnProjectile()
{
	check(CharacterOwner);

	const FRotator ProjRotation = GetFireRotation();
	const FVector ProjLocation = GetFireLocation() + ProjRotation.RotateVector(FireOffset);
	GetWorld()->SpawnActor<ABSProjectile>(ProjectileClass, ProjLocation, ProjRotation);
}

void ABSWeapon::PlayFireEffects()
{
	if (MuzzleEffect)
		MuzzleEffect->Activate(true);

	if(FireSound)
		UGameplayStatics::SpawnSoundAttached(FireSound, RootComponent);
}

void ABSWeapon::OnRep_ServerFired()
{
	PlayFireEffects();
}

void ABSWeapon::EndFireSequence()
{
	if (EndFireSequenceSound)
	{
		if (!HasAuthority())
		{
			UGameplayStatics::SpawnSoundAttached(EndFireSequenceSound, RootComponent);
			ServerEndFireSequence();
		}
		else
		{
			UBSNetworkUtils::PlaySound(EndFireSequenceSound, this, FVector::ZeroVector, EReplicationOption::AllButOWner);
		}
	}
}

void ABSWeapon::ServerEndFireSequence_Implementation()
{
	EndFireSequence();
}

bool ABSWeapon::ServerEndFireSequence_Validate()
{
	return true;
}

FRotator ABSWeapon::GetFireRotation_Implementation() const
{
	check(CharacterOwner);
	return CharacterOwner->GetViewRotation();
}

FVector ABSWeapon::GetFireLocation_Implementation() const
{
	return GetActorLocation();
}

// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "BattleStage.h"
#include "BSCharacter.h"
#include "BSProjectile.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"
#include "MotionControllerComponent.h"
#include "BSWeapon.h"
#include "UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

ABSCharacter::ABSCharacter()
	: Super(FObjectInitializer::Get().DoNotCreateDefaultSubobject(ACharacter::MeshComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// First person camera
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->RelativeLocation = FVector(-39.56f, 1.75f, 64.f);
	FirstPersonCamera->bUsePawnControlRotation = true;
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	
	// Setup first person mesh.
	// Only owner can see and does not replicate.
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->SetIsReplicated(false);
	FirstPersonMesh->SetupAttachment(FirstPersonCamera);
	FirstPersonMesh->SetCastShadow(false);
	FirstPersonMesh->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	FirstPersonMesh->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Setup character mesh.
	// Owner can not see and does replicate.
	CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	CharacterMesh->SetOwnerNoSee(true);
	CharacterMesh->SetIsReplicated(true);
	CharacterMesh->SetupAttachment(GetCapsuleComponent());
}

void ABSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABSCharacter, EquippedWeapon);
}

void ABSCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ABSCharacter::PossessedBy(AController* NewController)
{
	SetOwner(NewController);
	Super::PossessedBy(NewController);
}

void ABSCharacter::EquipWeapon()
{
	if (!HasAuthority())
	{
		if (AvailableWeapons.Num() > 0)
			ServerEquipWeapon();
	}
	else
	{
		ServerEquipWeapon_Implementation();
	}
}

void ABSCharacter::ServerEquipWeapon_Implementation()
{
	if (AvailableWeapons.Num() > 0)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = this;
		SpawnParams.Owner = this;
		EquippedWeapon = GetWorld()->SpawnActor<ABSWeapon>(AvailableWeapons[0], SpawnParams);
		EquippedWeapon->OnEquip(this);
	}
}

bool ABSCharacter::ServerEquipWeapon_Validate()
{
	return true;
}

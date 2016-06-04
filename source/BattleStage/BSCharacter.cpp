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
	: Super()
	, WeaponEquippedSocket(TEXT("GripPoint"))
{
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
	USkeletalMeshComponent* Mesh = GetMesh();
	Mesh->SetOwnerNoSee(true);
	Mesh->SetIsReplicated(true);
	Mesh->SetupAttachment(GetCapsuleComponent());
}

void ABSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABSCharacter, Weapon);
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

void ABSCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

bool ABSCharacter::IsFirstPerson() const
{
	return IsLocallyControlled();
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

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

	if (ActualDamage > 0)
	{
		Health -= ActualDamage;

		UE_LOG(BattleStage, Log, TEXT("Character %s took %f damage"), *GetName(), ActualDamage);

		if (Health <= 0)
		{
			Die(DamageEvent, EventInstigator, DamageCauser);
		}
		else
		{
			// #bstodo Implement hit event, non-dieing
		}
	}

	return ActualDamage;
}

bool ABSCharacter::CanDie() const
{
	return Health > 0 && !IsPendingKill();
}

void ABSCharacter::Die(struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// #bstodo Implement die event (ragdollies)
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

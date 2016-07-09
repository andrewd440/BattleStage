// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "Weapons/BSProjectileShot.h"
#include "Weapons/BSProjectile.h"
#include "Weapons/BSWeapon.h"

bool UBSProjectileShot::GetShotData(FShotData& OutShotData) const
{
	const ABSWeapon* const Weapon = GetWeapon();
	OutShotData.Start = Weapon->GetFireLocation();

	// Get a random weapon spread for the shot
	const int32 RandomSeed = FMath::Rand();
	FRandomStream SpreadStream(RandomSeed);
	const float BaseSpread = FMath::DegreesToRadians(Weapon->GetCurrentSpread());

	// Use spread to offset shot
	const FVector TrueAimDirection = Weapon->GetFireRotation().Vector();
	OutShotData.Direction = SpreadStream.VRandCone(TrueAimDirection, BaseSpread);

	OutShotData.bImpactNeeded = false;

	return true;
}

void UBSProjectileShot::InvokeShot(const FShotData& ShotData)
{
	SpawnProjectile(ShotData.Start, ShotData.Direction);
}

void UBSProjectileShot::SpawnProjectile(FVector Location, FVector_NetQuantize Direction) const
{
	if (ProjectileType)
	{
		ABSWeapon* const Weapon = GetWeapon();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Weapon;
		SpawnParams.Instigator = Weapon->GetCharacter();

		GetWorld()->SpawnActor<ABSProjectile>(ProjectileType, Location, Direction.Rotation(), SpawnParams);
	}
}
// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSProjectileShot.h"
#include "BSProjectile.h"
#include "BSWeapon.h"

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

void UBSProjectileShot::SpawnProjectile(const FVector& Location, const FVector& Direction) const
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
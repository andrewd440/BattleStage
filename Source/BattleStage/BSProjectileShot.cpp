// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSProjectileShot.h"
#include "BSProjectile.h"
#include "BSWeapon.h"

bool UBSProjectileShot::GetShotData(FShotData& OutShotData) const
{
	const ABSWeapon* const Weapon = GetWeapon();
	OutShotData.Start = Weapon->GetFireLocation();
	OutShotData.Direction = Weapon->GetFireRotation().Vector();
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
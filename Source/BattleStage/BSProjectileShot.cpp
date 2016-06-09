// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSProjectileShot.h"
#include "BSProjectile.h"
#include "BSWeapon.h"

void UBSProjectileShot::FireShot()
{
	ABSWeapon* const Weapon = GetWeapon();
	const FVector Location = Weapon->GetFireLocation();
	const FVector Direction = Weapon->GetFireRotation().Vector();

	if (Weapon->HasAuthority())
	{
		SpawnProjectile(Location, Direction);
		Weapon->NotifyFired();
	}
	else
	{
		ServerFireShot(Location, Direction);
	}
}

void UBSProjectileShot::SpawnProjectile(FVector Location, FVector_NetQuantize Direction) const
{
	if (ProjectileType)
	{
		ABSWeapon* const Weapon = GetWeapon();
		UWorld* World = Weapon->GetWorld();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Weapon;
		SpawnParams.Instigator = Weapon->GetCharacter();

		World->SpawnActor<ABSProjectile>(ProjectileType, Location, Direction.Rotation(), SpawnParams);
	}
}

void UBSProjectileShot::ServerFireShot_Implementation(FVector Location, FVector_NetQuantize Direction) const
{
	SpawnProjectile(Location, Direction);
}

bool UBSProjectileShot::ServerFireShot_Validate(FVector Location, FVector_NetQuantize Direction)
{
	return true;
}
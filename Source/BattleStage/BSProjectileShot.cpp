// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSProjectileShot.h"
#include "BSProjectile.h"

void UBSProjectileShot::Fire(const FShotTypeFireParams& FireParams) const
{
	if (!FireParams.Owner)
	{
		UE_LOG(BattleStage, Warning, TEXT("UBSProjectileShot::Fire() called without a valid Owner."))
	}
	else if(ProjectileClass)
	{
		UWorld* World = FireParams.Owner->GetWorld();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = FireParams.Owner;
		SpawnParams.Instigator = FireParams.Instigator;

		World->SpawnActor<ABSProjectile>(ProjectileClass, FireParams.Position, FireParams.Direction.Rotation(), SpawnParams);
	}
}

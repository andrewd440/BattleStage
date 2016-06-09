// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSInstantShot.h"
#include "BSWeapon.h"

static const float MAX_SHOT_RANGE = 10000.f;

void UBSInstantShot::FireShot()
{
	//const ABSWeapon* const Weapon
	//const FVector& FireStart = FireParams.Position;
	//const FVector FireEnd = FireStart + FireParams.Direction * MAX_SHOT_RANGE;

	//UWorld* World = FireParams.Owner->GetWorld();

	//FHitResult FireHit;
	//const FCollisionQueryParams QueryParams(NAME_None, false, FireParams.Owner);
	//World->LineTraceSingleByChannel(FireHit, FireStart, FireEnd, WEAPON_CHANNEL, QueryParams);

	//FireHit.Actor->
}

void UBSInstantShot::PlayFireEffects()
{
	//if (TrailFX)
	//{
	//	UParticleSystemComponent* TrailFXComponent =  UGameplayStatics::SpawnEmitterAtLocation(FireParams.Owner->GetWorld(), TrailFX, FireParams.Position);

	//	if (TrailEndParam != NAME_None)
	//	{
	//		TrailFXComponent->SetVectorParameter(TrailEndParam, FireParams.Position + FireParams.Direction * MAX_SHOT_RANGE);
	//	}
	//}
}

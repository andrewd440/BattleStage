// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSImpactEffect.h"
#include "Class.h"

void UBSImpactEffect::SpawnEffect(UWorld* World, const FHitResult& Hit) const
{
	if (!World)
	{
		UE_LOG(BattleStage, Warning, TEXT("UBSImpactEffect::SpawnEffect World parameter is invalid."));
	}
	else
	{
		const FVector Location = Hit.ImpactPoint;
		const FRotator Rotation = Hit.ImpactNormal.Rotation();

		if (ImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(World, ImpactParticles, Location, Rotation);
		}

		if (ImpactSound)
		{
			UGameplayStatics::SpawnSoundAtLocation(World, ImpactSound, Location, Rotation);
		}

		if (DecalInfo.Material)
		{
			const FRotator DecalRotation = Rotation;

			// #bstodo Apply random rotation to decal
			UGameplayStatics::SpawnDecalAttached(DecalInfo.Material, DecalInfo.DecalSize, Hit.Component.Get(), Hit.BoneName,
				Location, DecalRotation, EAttachLocation::KeepWorldPosition, DecalInfo.LifeSpan);
		}
	}
}

void UBSImpactEffect::BeginDestroy()
{
	Super::BeginDestroy();

	if (GetWorld() != nullptr && GetWorld()->WorldType == EWorldType::Game)
	{
		if (GAreScreenMessagesEnabled)
		{
			GEngine->AddOnScreenDebugMessage((uint64)-1, 3.0f, FColor::Red, 
				*FString::Printf(TEXT("UBSImpactEffect should not be spawned. Use SpawnEffect on default class. %s"), *GetName()));
		}

		UE_LOG(BattleStage, Warning, TEXT("UBSImpactEffect should not be instanced. Use DefaultObject instead."));
	}	
}

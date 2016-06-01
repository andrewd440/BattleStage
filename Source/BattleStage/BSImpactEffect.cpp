// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSImpactEffect.h"


// Sets default values
ABSImpactEffect::ABSImpactEffect()
{
	bReplicates = false;
	InitialLifeSpan = 1.0f;
}

void ABSImpactEffect::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAttached(ImpactParticles, RootComponent);
	}

	if (ImpactSound)
	{
		UGameplayStatics::SpawnSoundAttached(ImpactSound, RootComponent);
	}

	if (DecalInfo.Material)
	{
		const FRotator DecalRotation = SurfaceHit.ImpactNormal.Rotation();

		// #bstodo Apply random rotation to decal
		UGameplayStatics::SpawnDecalAttached(DecalInfo.Material, DecalInfo.DecalSize, SurfaceHit.Component.Get(), SurfaceHit.BoneName,
			SurfaceHit.ImpactPoint, DecalRotation, EAttachLocation::KeepWorldPosition, DecalInfo.LifeSpan);
	}
}


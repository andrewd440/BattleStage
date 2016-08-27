// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSExplosion.h"

ABSExplosion::ABSExplosion(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true; // Needed for auto destroy
	bAutoDestroyWhenFinished = true;
}

void ABSExplosion::BeginPlay()
{
	Super::BeginPlay();

	if (Particles)
	{
		UGameplayStatics::SpawnEmitterAttached(Particles, RootComponent);
	}

	if (Sound)
	{
		UGameplayStatics::SpawnSoundAttached(Sound, RootComponent);
	}

	if (DecalInfo.Material)
	{		
		UGameplayStatics::SpawnDecalAttached(DecalInfo.Material, 
			DecalInfo.DecalSize, 
			RootComponent, NAME_None, 
			FVector::ZeroVector, 
			FRotator::ZeroRotator, 
			EAttachLocation::KeepRelativeOffset, 
			DecalInfo.LifeSpan);
	}
	
}

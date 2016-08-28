// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BSTypes.h"

#include "BSExplosion.generated.h"

/**
 * Uses to spawn explosion effects in the environment.
 */
UCLASS(Abstract)
class BATTLESTAGE_API ABSExplosion : public AActor
{
	GENERATED_BODY()

public:

	ABSExplosion(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** AActor Interface Begin */
	virtual void BeginPlay() override;
	/** AActor Interface End */

protected:
	UPROPERTY(EditDefaultsOnly, Category = Impact)
	class USoundBase* Sound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Impact)
	class UParticleSystem* Particles = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Impact)
	FDecalInfo DecalInfo;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BSTypes.h"


#include "BSImpactEffect.generated.h"

UCLASS()
class BATTLESTAGE_API ABSImpactEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABSImpactEffect();

	/** AActor interface */
	virtual void PostInitializeComponents() override;
	/** AActor interface end */

	UPROPERTY(EditDefaultsOnly, Category = Impact)
	class USoundBase* ImpactSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Impact)
	class UParticleSystem* ImpactParticles = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Impact)
	FDecalInfo DecalInfo;

	UPROPERTY(BlueprintReadOnly, Category = Impact)
	FHitResult SurfaceHit;
};

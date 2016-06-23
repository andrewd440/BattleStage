// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject.h"
#include "BSTypes.h"

#include "BSImpactEffect.generated.h"

UCLASS(Blueprintable, NotPlaceable, Config = Game)
class BATTLESTAGE_API UBSImpactEffect : public UObject
{
	GENERATED_BODY()
	
public:	
	UFUNCTION(BlueprintCallable, Category = ImpactEffect)
	virtual void SpawnEffect(UWorld* World, const FHitResult& Hit) const;

	/** UObject interface */
	virtual void BeginDestroy() override;
	/** UObject interface end*/

protected:
	UPROPERTY(EditDefaultsOnly, Category = Impact)
	class USoundBase* ImpactSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Impact)
	class UParticleSystem* ImpactParticles = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Impact)
	FDecalInfo DecalInfo;
};

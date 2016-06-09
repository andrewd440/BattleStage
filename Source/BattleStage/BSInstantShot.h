// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BSShotType.h"
#include "BSInstantShot.generated.h"


/**
 * 
 */
UCLASS()
class BATTLESTAGE_API UBSInstantShot : public UBSShotType
{
	GENERATED_BODY()
	
public:
	virtual void FireShot() override;

protected:
	virtual void PlayFireEffects();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ProjectileShot)
	TSubclassOf<class ABSImpactEffect> ImpactEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ProjectileShot)
	UParticleSystem* TrailFX = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ProjectileShot)
	FName TrailEndParam = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ProjectileShot)
	TSubclassOf<class UDamageType> DamageType = UDamageType::StaticClass();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ProjectileShot)
	float BaseDamage = 1.f;
};

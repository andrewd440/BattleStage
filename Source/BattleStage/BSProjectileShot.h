// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BSShotType.h"
#include "BSProjectileShot.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESTAGE_API UBSProjectileShot : public UBSShotType
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = ShotType)
	virtual void Fire(const FShotTypeFireParams& FireParams) const override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ShotType)
	TSubclassOf<class ABSProjectile> ProjectileClass = nullptr;
};

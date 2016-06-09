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

	/** UBSShotType interface */
	UFUNCTION(BlueprintCallable, Category = ProjectileShot)
	virtual void FireShot() override;
	/** UBSShotType interface end */

protected:
	UFUNCTION(Server, Unreliable, WithValidation)
	virtual void ServerFireShot(FVector Location, FVector_NetQuantize Direction) const;

	virtual void SpawnProjectile(FVector Location, FVector_NetQuantize Direction) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ProjectileShot)
	TSubclassOf<class ABSProjectile> ProjectileType = nullptr;
};

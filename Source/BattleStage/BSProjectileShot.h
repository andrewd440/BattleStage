// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BSShotType.h"
#include "BSProjectileShot.generated.h"

/**
 * Shot type for projectile based shots.
 */
UCLASS(Abstract)
class BATTLESTAGE_API UBSProjectileShot : public UBSShotType
{
	GENERATED_BODY()
	
public:

	/** UBSShotType interface */
	virtual bool GetShotData(FShotData& OutShotData) const override;
	virtual void InvokeShot(const FShotData& ShotData) override;
	/** UBSShotType interface end */

protected:
	/** Spawns a projectile of ProjectileType */
	virtual void SpawnProjectile(FVector Location, FVector_NetQuantize Direction) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ProjectileShot)
	TSubclassOf<class ABSProjectile> ProjectileType = nullptr;
};

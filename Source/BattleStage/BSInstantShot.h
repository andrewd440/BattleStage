// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BSShotType.h"
#include "BSInstantShot.generated.h"


USTRUCT()
struct FInstantShotRep
{
	GENERATED_BODY()

	UPROPERTY()
	FVector_NetQuantize10 Target = FVector::ZeroVector;

	UPROPERTY()
	uint32 FireToggle : 1;

	bool operator==(const FInstantShotRep& Other)
	{
		return Target == Target && FireToggle == FireToggle;
	}

	bool operator!=(const FInstantShotRep& Other)
	{
		return Target != Target || FireToggle != FireToggle;
	}
};

/**
 * Shot type of instant hit shots.
 */
UCLASS(Abstract)
class BATTLESTAGE_API UBSInstantShot : public UBSShotType
{
	GENERATED_BODY()
	
public:
	/** UBSShotType interface */
	virtual bool GetShotData(FShotData& OutShotData) const override;
	virtual void PreInvokeShot(const FShotData& ShotData) override;
	virtual void InvokeShot(const FShotData& ShotData) override;
	/** UBSShotType interface end */

	/** UObject interface */
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const override;
	/** UObject interface end */

protected:
	void PlayTrailEffects(const FVector& Start, const FVector& End) const;

	void PlayImpactEffects(const FHitResult& Hit) const;

	void ProcessHit(const FShotData& ShotData);

	void ProcessMiss(const FShotData& ShotData);

	void RespondValidHit(const FShotData& ShotData);

	void SimulateFire(const FVector& Target) const;

	FHitResult WeaponTrace(const FVector& Start, const FVector& End) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ProjectileShot)
	TSubclassOf<class UBSImpactEffect> ImpactEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ProjectileShot)
	UParticleSystem* TrailFX = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ProjectileShot)
	FName TrailEndParam = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ProjectileShot)
	TSubclassOf<class UDamageType> DamageType = UDamageType::StaticClass();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ProjectileShot)
	float BaseDamage = 1.f;

private:
	UPROPERTY(Transient, ReplicatedUsing = OnRep_ShotRep)
	FInstantShotRep ShotRep;

private:
	UFUNCTION()
	void OnRep_ShotRep();
};

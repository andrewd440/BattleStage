// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/BSShotType.h"
#include "BSInstantShot.generated.h"


//-----------------------------------------------------------------
// Shot data used to replicate effects to remotes for 
// UBSInstantShot
//-----------------------------------------------------------------
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

	/**
	* Processes a shot hit event from clients. Intended to only be called by the server to respond
	* to a possible hit by a client shot trace. The hit will be validated and processed on the server.
	* 
	* @param ShotData	The shot data from the hit.
	*/
	void ProcessHit(const FShotData& ShotData);

	/**
	* Processes a shot miss event from clients. Intended to only be called by the server to respond
	* to shot misses.
	* 
	* @param ShotData	The shot data from the invoked shot.
	*/
	void ProcessMiss(const FShotData& ShotData);

	/**
	* Responds to a verified hit on the server. Should be only called on the server. Applies any
	* responses from a verified hit and notifies remotes of the hit.
	* 
	* @param ShotData	The shot data from the invoked shot.
	*/
	void RespondValidHit(const FShotData& ShotData);
	
	/**
	* Simulates shot effects to a target location. Only plays visual and audible effects.
	*/
	void SimulateFire(const FVector& Target) const;

	/**
	* Performs a weapon trace from a start location to a end location.
	*/
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

private:
	// Shot data used to replicate shot effects on remotes
	UPROPERTY(Transient, ReplicatedUsing = OnRep_ShotRep)
	FInstantShotRep ShotRep;

private:
	UFUNCTION()
	void OnRep_ShotRep();
};

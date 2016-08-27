// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject.h"
#include "BSTypes.h"

#include "BSImpactEffect.generated.h"

/**
 * Represents an impact effect that will be played on a 
 * specific material type with BSImpactEffect.
 */
USTRUCT()
struct FMaterialEffect
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	class USoundBase* Sound = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* Particles = nullptr;
};

/**
* An impact effect that can be used to visually indicate an impact on
* a specific physical material surface. Used for impacts with instant
* shot types.
* 
* Should never by spawned. Instead, use SpawnEffect on the default object.
*/
UCLASS(Blueprintable, NotPlaceable, Config = Game)
class BATTLESTAGE_API UBSImpactEffect : public UObject
{
	GENERATED_BODY()
	
public:	

	/**
	* Spawns the impact effect.
	* 
	* @param World	The world context.
	* @param Hit	The hit impact to play to effect for.
	*/
	UFUNCTION(BlueprintCallable, Category = ImpactEffect)
	virtual void SpawnEffect(UWorld* World, const FHitResult& Hit) const;

	//-----------------------------------------------------------------
	// UObject Interface 
	//-----------------------------------------------------------------
	virtual void BeginDestroy() override;
	//-----------------------------------------------------------------
	// UObject Interface End
	//-----------------------------------------------------------------

protected:
	/** Decal information used to spawn impact decals */
	UPROPERTY(EditDefaultsOnly, Category = Impact)
	FDecalInfo DecalInfo;

	/** Effects play for specific physical materials */
	UPROPERTY(EditDefaultsOnly, Category = Impact)
	FMaterialEffect SurfaceEffects[SurfaceType_Max];

private:
	/** Gets the surface effect for the specified physical material */
	const FMaterialEffect& GetEffect(TWeakObjectPtr<UPhysicalMaterial> Material) const;
};

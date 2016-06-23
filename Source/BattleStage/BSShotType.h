// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "BSShotType.generated.h"

//-------------------------------------------------------------------------------------
// Parameters used to gather and use shot data by UBSShotType when
// firing a shot and replicating data to the server, when invoked by
// a client.
//-------------------------------------------------------------------------------------
USTRUCT()
struct FShotData
{
	GENERATED_USTRUCT_BODY()

	// The start location of the shot
	UPROPERTY()
	FVector_NetQuantize10 Start;

	// The direction of the shot
	UPROPERTY()
	FVector_NetQuantizeNormal Direction;

	// The hit result of the shot location
	UPROPERTY()
	FHitResult Impact;

	// If Impact is needed for the shot result
	UPROPERTY()
	bool bImpactNeeded;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		bOutSuccess = true;

		bool bOutSuccessLocal = true;

		Start.NetSerialize(Ar, Map, bOutSuccessLocal);
		bOutSuccess &= bOutSuccessLocal;
		Direction.NetSerialize(Ar, Map, bOutSuccessLocal);
		bOutSuccess &= bOutSuccessLocal;

		Ar.SerializeBits(&bImpactNeeded, 1);

		if (bImpactNeeded) // Don't send the Impact if not needed.
		{
			Impact.NetSerialize(Ar, Map, bOutSuccessLocal);
			bOutSuccess &= bOutSuccessLocal;
		}

		return true;
	}
};

template<>
struct TStructOpsTypeTraits< FShotData > : public TStructOpsTypeTraitsBase
{
	enum
	{
		WithNetSerializer = true,
	};
};

/**
 * Base for any type of shot that can be fired from a weapon.
 * Provides an interface for getting the initial shot data from a shot, which
 * can be called on the client or server, notifing a preshot, and invoking a 
 * shot on the server.
 */
UCLASS(Blueprintable, Abstract, NotPlaceable, Config = Game, Within=BSWeapon)
class BATTLESTAGE_API UBSShotType : public UObject
{
	GENERATED_BODY()

public:

	/**
	* [Client/Server]
	* Gets shot data associated with a shot being fired from the owning weapon.
	* 
	* @param ShotData	Output of the shot data.
	* 
	* @returns True if a valid shot data could be retrieved. If false, the shot should
	*			not be invoked.
	*/
	UFUNCTION(BlueprintCallable, Category = ShotType)
	virtual bool GetShotData(FShotData& OutShotData) const PURE_VIRTUAL(UBSShotType::GetShotData, return false;);

	/**
	* [Client/Server]
	* To be called prior to InvokeShot() by the connection that called GetShotData().
	* Used to play effects solely on the connection that is provoking the shot.
	* 
	* @param ShotData	The shot data provided by GetShotData().
	*/
	UFUNCTION(BlueprintCallable, Category = ShotType)
	virtual void PreInvokeShot(const FShotData& ShotData) {}

	/**
	* [Server]
	* Fires the actual shot. Deals damage, replicates events to remotes, verifies hits, etc.
	* 
	* @param ShotData	The shot data provided by GetShotData().
	* @returns
	*/
	UFUNCTION(BlueprintCallable, Category = ShotType)
	virtual void InvokeShot(const FShotData& ShotData) PURE_VIRTUAL(UBSShotType::InvokeShot, );

	/** UObject interface */
	virtual class UWorld* GetWorld() const override;
	virtual bool IsSupportedForNetworking() const override;
	/** UObject interface end */

protected:

	/** The owning weapon */
	class ABSWeapon* GetWeapon() const;
};
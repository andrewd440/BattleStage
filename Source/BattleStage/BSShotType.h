// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "BSWeapon.h"
#include "BSShotType.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, Abstract, NotPlaceable, Config = Game)
class BATTLESTAGE_API UBSShotType : public UObject
{
	GENERATED_BODY()

public:

	/**
	*
	* @param FireParams
	* @returns
	*/
	UFUNCTION(BlueprintCallable, Category = ShotType)
	virtual void FireShot() PURE_VIRTUAL(UBSShotType::FireShot,);

protected:
	ABSWeapon* GetWeapon() const;
};


FORCEINLINE ABSWeapon* UBSShotType::GetWeapon() const { check(Cast<ABSWeapon>(GetOuter())); return static_cast<ABSWeapon*>(GetOuter()); }
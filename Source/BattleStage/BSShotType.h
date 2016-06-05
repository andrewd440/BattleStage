// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "BSShotType.generated.h"

class ABSWeapon;

//-----------------------------------------------------------------
// Parameters used when fire shots through shot types.
//-----------------------------------------------------------------
USTRUCT()
struct FShotTypeFireParams
{
	GENERATED_USTRUCT_BODY()

	AActor* Owner = nullptr;
	APawn* Instigator = nullptr;
	FVector Position = FVector::ZeroVector;
	FVector Direction = FVector::ForwardVector;

	FShotTypeFireParams() = default;

	FShotTypeFireParams(AActor* InOwner, APawn* InInstigator, const FVector& InPosition, const FVector& InDirection)
		: Owner(InOwner)
		, Instigator(InInstigator)
		, Position(InPosition)
		, Direction(InDirection) 
	{}
};

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
	* @note Implementers, default objects will be used to fire shots, so no internal state should
	*		be changed on Fire().
	* @param FireParams
	* @returns
	*/
	UFUNCTION(BlueprintCallable, Category = ShotType)
	virtual void Fire(const FShotTypeFireParams& FireParams) const PURE_VIRTUAL(UBSShotType::Fire,);
};

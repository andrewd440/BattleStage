// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "BSHUD.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNotifyWeaponHitEvent);

/**
 * 
 */
UCLASS()
class BATTLESTAGE_API ABSHUD : public AHUD
{
	GENERATED_BODY()
	
public:	
	/** Notifies the HUD when a controlled weapon has hit a character. */
	void NotifyWeaponHit();

public:
	/** Broadcasted when a weapon hit has occurred. */
	UPROPERTY(BlueprintAssignable, Category = HUD)
	FOnNotifyWeaponHitEvent OnNotifyWeaponHit;
};

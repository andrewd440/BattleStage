// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BSWeapon.h"
#include "BSWeaponBlaster.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESTAGE_API ABSWeaponBlaster : public ABSWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Fire() override;

};

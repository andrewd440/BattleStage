// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "BSShotType.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESTAGE_API UBSShotType : public UObject
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = ShotType)
	virtual void FireShot() PURE_VIRTUAL(UBSShotType::FireShot,);
	
};

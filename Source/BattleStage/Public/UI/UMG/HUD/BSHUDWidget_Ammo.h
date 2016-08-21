// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BSHUDWidget.h"
#include "BSHUDWidget_Ammo.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class BATTLESTAGE_API UBSHUDWidget_Ammo : public UBSHUDWidget
{
	GENERATED_BODY()
	
	
protected:
	/** UUserWidget Interface Begin */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;
	/** UUserWidget Interface End */

protected:
	UPROPERTY(BlueprintReadOnly, Category = AmmoWidget)
	int32 RemainingAmmo;

	UPROPERTY(BlueprintReadOnly, Category = AmmoWidget)
	int32 RemainingClip;

	UPROPERTY(BlueprintReadOnly, Category = AmmoWidget)
	TWeakObjectPtr<class ABSCharacter> BSCharacter;
};

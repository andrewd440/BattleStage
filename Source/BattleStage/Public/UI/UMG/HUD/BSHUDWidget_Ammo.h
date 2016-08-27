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
	/** UUserWidget Interface End */

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* RemainingAmmoText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RemainingClipText;

private:
	int32 RemainingAmmo = 0; // Cached ammo count that is reflected on widget
	int32 RemainingClip = 0; // Cached ammo count that is reflected on widget
};

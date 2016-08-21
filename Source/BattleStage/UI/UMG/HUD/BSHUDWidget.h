// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/UMG/BSUserWidget.h"
#include "BSHUDWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class BATTLESTAGE_API UBSHUDWidget : public UBSUserWidget
{
	GENERATED_BODY()
	
public:
	UBSHUDWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintImplementableEvent, Category = HUDWidget)
	void OnGameStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = HUDWidget)
	void OnGameEnded();
};

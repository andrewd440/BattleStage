// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/UMG/BSUserWidget.h"
#include "BSHostGameWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class BATTLESTAGE_API UBSHostGameWidget : public UBSUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = HostGameWidget)
	bool HostGame(const FString& MatchOptions);
};

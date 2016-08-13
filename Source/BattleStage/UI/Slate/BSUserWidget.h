// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "BSUserWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class BATTLESTAGE_API UBSUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	/** Gets the game instance associated with this widget */
	class UBSGameInstance* GetGameInstance() const;
};

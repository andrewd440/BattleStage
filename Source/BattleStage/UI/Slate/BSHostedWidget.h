// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "BSHostedWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class BATTLESTAGE_API UBSHostedWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	bool SetWidgetHost(class UBSUIHost* Host);

protected:
	UPROPERTY(EditDefaultsOnly, Category = HostedWidget)
	TSubclassOf<UBSUIHost> HostType;

	UPROPERTY(BlueprintReadOnly, Category = HostedWidget)
	UBSUIHost* Host;
};

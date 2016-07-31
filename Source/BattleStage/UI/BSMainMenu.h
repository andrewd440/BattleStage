// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BSUIHost.h"
#include "BSServerBrowserHost.h"
#include "BSMainMenu.generated.h"

class UBSServerBrowserHost;

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class BATTLESTAGE_API UBSMainMenu : public UBSUIHost
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = MainMenu)
	bool HostGame();

	UFUNCTION(BlueprintCallable, Category = MainMenu)
	UBSServerBrowserHost* OpenServerBrowser();

	//UFUNCTION(BlueprintCallable, Category = MainMenu)
	//bool OpenReplays();

	//UFUNCTION(BlueprintCallable, Category = MainMenu)
	//bool OpenSettings();

	/** UBSUIHost Interface Begin */
	virtual void Open() override;
	/** UBSUIHost Interface End */

protected:
	UPROPERTY(EditAnywhere, Category = MainMenu)
	TSubclassOf<class UBSHostedWidget> MainMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = MainMenu)
	TSubclassOf<UBSServerBrowserHost> ServerBrowserHostClass;

	UBSServerBrowserHost* ServerBrowserHost = nullptr;

	UBSUIHost* ActiveSubHost = nullptr;

private:
	UBSHostedWidget* MainMenuWidget = nullptr;
};

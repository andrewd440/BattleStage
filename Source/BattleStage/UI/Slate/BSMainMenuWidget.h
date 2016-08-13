// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/Slate/BSUserWidget.h"
#include "BSMainMenuWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class BATTLESTAGE_API UBSMainMenuWidget : public UBSUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = MainMenu)
	bool HostGame();

	UFUNCTION(BlueprintCallable, Category = MainMenu)
	void OpenServerBrowser();

	UFUNCTION(BlueprintCallable, Category = MainMenu)
	void Quit();
	
protected:
	/** 
	 * Gets the panel that submenus will be added to for this widget. 
	 * 
	 * Should be used by derived classes to specify where to add submenus
	 * in the main menu widget.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = MainMenu)
	class UContentWidget* GetSubmenuPanel();

protected:
	UPROPERTY(EditDefaultsOnly, Category = MainMenu)
	TSubclassOf<class UBSServerBrowserWidget> ServerBrowserClass;

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBSServerBrowserWidget* ServerBrowserWidget = nullptr;
};

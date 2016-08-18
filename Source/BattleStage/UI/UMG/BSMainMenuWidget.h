// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/UMG/BSUserWidget.h"
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
	void Quit();
	
	UFUNCTION(BlueprintCallable, Category = MainMenu)
	void ShowSubMenu(TSubclassOf<UBSUserWidget> MenuType);

protected:
	/** 
	 * Gets the panel that sub-menus will be added to for this widget. 
	 * 
	 * Should be used by derived classes to specify where to add sub-menus
	 * in the main menu widget.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = MainMenu)
	class UContentWidget* GetSubMenuPanel();

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBSUserWidget* ActiveSubMenu = nullptr;
};

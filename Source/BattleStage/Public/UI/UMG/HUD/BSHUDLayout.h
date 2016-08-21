// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"

#include "BSHUDLayout.generated.h"

class UBSHUDWidget;

/**
 * 
 */
UCLASS(Abstract)
class BATTLESTAGE_API UBSHUDLayout : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Gets the main layout panel that contains all HUD widgets */
	UPanelWidget* GetLayoutPanel() const;

	/** Gets all HUD widgets held within this layout */
	const TArray<UBSHUDWidget*>& GetAllHUDWidgets() const;

protected:
	virtual void NativeConstruct() override;

private:
	/** The layout panel that contains all HUD widgets */
	UPROPERTY(meta = (BindWidget))
	UPanelWidget* LayoutPanel;

	/** All HUD widgets within this layout */
	TArray<UBSHUDWidget*> HUDWidgets;
};

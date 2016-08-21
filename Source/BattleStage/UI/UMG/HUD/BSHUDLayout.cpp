// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "Blueprint/WidgetTree.h"

#include "BSHUDLayout.h"
#include "BSHUDWidget.h"


UPanelWidget* UBSHUDLayout::GetLayoutPanel() const
{
	return LayoutPanel;
}

const TArray<UBSHUDWidget*>& UBSHUDLayout::GetAllHUDWidgets() const
{
	return HUDWidgets;
}

void UBSHUDLayout::NativeConstruct()
{
	Super::NativeConstruct();

	// Gather all HUD widgets
	WidgetTree->ForWidgetAndChildren(LayoutPanel, [&](UWidget* Widget)
	{
		if (UBSHUDWidget* HUDWidget = Cast<UBSHUDWidget>(Widget))
		{
			HUDWidgets.Add(HUDWidget);
		}
	});
}

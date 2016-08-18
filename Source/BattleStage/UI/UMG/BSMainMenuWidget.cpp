// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSMainMenuWidget.h"
#include "BSServerBrowserWidget.h"
#include "BSHostGameWidget.h"
#include "BSGameInstance.h"
#include "ContentWidget.h"

void UBSMainMenuWidget::Quit()
{
	GetOwningPlayer()->ConsoleCommand(TEXT("Quit"));
}

void UBSMainMenuWidget::ShowSubMenu(TSubclassOf<UBSUserWidget> MenuType)
{
	UContentWidget* const SubMenuPanel = GetSubMenuPanel();
	if (MenuType && SubMenuPanel)
	{
		// Only open if not already open
		if (SubMenuPanel->GetContentSlot() == nullptr ||
			SubMenuPanel->GetContentSlot()->Content->StaticClass() != MenuType)
		{
			SubMenuPanel->ClearChildren();

			ActiveSubMenu = CreateWidget<UBSUserWidget>(GetOwningPlayer(), MenuType);
			SubMenuPanel->AddChild(ActiveSubMenu);
		}
	}
	else
	{
		UE_LOG(BattleStageUI, Warning, TEXT("No %s assigned to open main menu sub-menu."), MenuType ? TEXT("MenuType") : TEXT("SubmenuPanel"));
	}
}

UContentWidget* UBSMainMenuWidget::GetSubMenuPanel_Implementation()
{
	return nullptr;
}

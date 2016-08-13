// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSMainMenuWidget.h"
#include "BSServerBrowserWidget.h"
#include "BSGameInstance.h"
#include "ContentWidget.h"


bool UBSMainMenuWidget::HostGame()
{
	UBSGameInstance* GameInstance = GetGameInstance();

	bool bSuccess = false;
	if (GameInstance)
	{
		bSuccess = GameInstance->HostSession(GetOwningLocalPlayer(), TEXT("/Game/Maps/DemoMap?game=DM?listen"));
	}

	return bSuccess;
}

void UBSMainMenuWidget::OpenServerBrowser()
{
	UContentWidget* const SubmenuPanel = GetSubmenuPanel();
	if (ServerBrowserClass && SubmenuPanel)
	{
		// Only open if not already open
		if (SubmenuPanel->GetContentSlot() == nullptr || 
			SubmenuPanel->GetContentSlot()->Content != ServerBrowserWidget)
		{
			SubmenuPanel->ClearChildren();

			ServerBrowserWidget = CreateWidget<UBSServerBrowserWidget>(GetOwningPlayer(), ServerBrowserClass);
			SubmenuPanel->AddChild(ServerBrowserWidget);			
		}
	}
	else
	{
		UE_LOG(BattleStageUI, Warning, TEXT("No %s assigned to open server browser widget."), ServerBrowserClass ? TEXT("ServerBrowserClass") : TEXT("SubmenuPanel"));
	}
}

void UBSMainMenuWidget::Quit()
{
	GetOwningPlayer()->ConsoleCommand(TEXT("Quit"));
}

UContentWidget* UBSMainMenuWidget::GetSubmenuPanel_Implementation()
{
	return nullptr;
}

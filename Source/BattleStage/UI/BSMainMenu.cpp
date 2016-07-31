// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSMainMenu.h"
#include "BSGameInstance.h"
#include "../UMG/Public/Blueprint/UserWidget.h"
#include "Slate/BSHostedWidget.h"


bool UBSMainMenu::HostGame()
{
	const FLocalPlayerContext& PlayerContext = GetLocalPlayerContext();
	UBSGameInstance* GameInstance = GetGameInstance();

	bool bSuccess = false;
	if (GameInstance)
	{
		bSuccess = GameInstance->HostSession(PlayerContext.GetLocalPlayer(), TEXT("/Game/Maps/DemoMap?game=DM?listen"));
	}

	return bSuccess;
}

UBSServerBrowserHost* UBSMainMenu::OpenServerBrowser()
{
	// Close any existing hosts that are not the server browser
	if (ActiveSubHost && ActiveSubHost != ServerBrowserHost)
	{
		ActiveSubHost->Close();
	}

	// Create browser if needed
	if (ServerBrowserHost == nullptr)
	{
		TSubclassOf<UBSServerBrowserHost> HostClass = ServerBrowserHostClass ? ServerBrowserHostClass : UBSServerBrowserHost::StaticClass();
		ServerBrowserHost = CreateSubHost<UBSServerBrowserHost>(HostClass, TEXT("ServerBrowserHost"));
	}

	// Open the browser
	ActiveSubHost = ServerBrowserHost;
	ServerBrowserHost->Open();
	return ServerBrowserHost;
}

void UBSMainMenu::Open()
{
	if (MainMenuWidget == nullptr)
	{
		MainMenuWidget = CreateHostedWidget<UBSHostedWidget>(MainMenuWidgetClass);
	}

	if (MainMenuWidget && !MainMenuWidget->IsInViewport())
	{
		MainMenuWidget->AddToViewport();
	}
}

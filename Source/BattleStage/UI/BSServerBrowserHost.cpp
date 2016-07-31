// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSServerBrowserHost.h"
#include "Slate/BSHostedWidget.h"
#include "GameModes/BSGameMode.h"
#include "BSGameInstance.h"
#include "Online/BSGameSession.h"
#include "OnlineSessionSettings.h"

bool UBSServerBrowserHost::SearchForSessions()
{
	bool bWasSuccessful = false;
	UBSGameInstance* const GameInstance = GetGameInstance();

	if (GameInstance)
	{
		const FLocalPlayerContext& PlayerContext = GetLocalPlayerContext();
		bWasSuccessful = GameInstance->FindSessions(PlayerContext.GetLocalPlayer(), FString{});
	}
	
	return bWasSuccessful;
}

TArray<FServerBrowserRecord> UBSServerBrowserHost::GetSearchResults()
{
	TArray<FServerBrowserRecord> SearchResults;

	const FLocalPlayerContext& PlayerContext = GetLocalPlayerContext();

	ABSGameMode* GameMode =  Cast<ABSGameMode>(PlayerContext.GetWorld()->GetAuthGameMode());
	
	if (GameMode)
	{
		ABSGameSession* GameSession = Cast<ABSGameSession>(GameMode->GameSession);

		if (GameSession)
		{
			const auto& Results = GameSession->GetSearchResults();
			
			for (const auto& Result : Results)
			{
				FServerBrowserRecord SessionInfo;
				SessionInfo.Name = Result.Session.OwningUserName;
				SessionInfo.PingInMs = Result.PingInMs;
				SessionInfo.MaxConnections = Result.Session.SessionSettings.NumPublicConnections;
				SessionInfo.AvailableConnections = Result.Session.NumOpenPublicConnections;

				SearchResults.Push(SessionInfo);
			}
		}
	}

	return SearchResults;
}

void UBSServerBrowserHost::Open()
{
	if (BrowserWidget == nullptr)
	{
		BrowserWidget = CreateHostedWidget<UBSHostedWidget>(BrowserWidgetClass);
	}

	if (BrowserWidget && !BrowserWidget->IsInViewport())
	{
		BrowserWidget->AddToViewport();
	}
}

void UBSServerBrowserHost::Close()
{
	if (BrowserWidget)
	{
		BrowserWidget->RemoveFromParent();
		BrowserWidget = nullptr;
	}

	Super::Close();
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSServerBrowserHost.h"
#include "Slate/BSHostedWidget.h"
#include "GameModes/BSGameMode.h"
#include "BSGameInstance.h"
#include "Online/BSGameSession.h"
#include "OnlineSessionSettings.h"



UBSServerBrowserHost::UBSServerBrowserHost(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
	, SearchStatus(EOnlineAsyncTaskState::NotStarted)
{

}

bool UBSServerBrowserHost::SearchForSessions()
{
	bool bWasSuccessful = false;
	UBSGameInstance* const GameInstance = GetGameInstance();

	if (GameInstance)
	{
		const FLocalPlayerContext& PlayerContext = GetLocalPlayerContext();
		bWasSuccessful = GameInstance->FindSessions(PlayerContext.GetLocalPlayer(), FString{});
	}

	UpdateSearchStatus();
	
	return bWasSuccessful;
}

const TArray<FServerListEntry>& UBSServerBrowserHost::GetSearchResults() const
{
	return CurrentServerList;
}

bool UBSServerBrowserHost::UpdateSearchStatus()
{
	bool bChanged = false;

	const FLocalPlayerContext& PlayerContext = GetLocalPlayerContext();

	ABSGameMode* const GameMode = Cast<ABSGameMode>(PlayerContext.GetWorld()->GetAuthGameMode());

	if (GameMode)
	{
		if (ABSGameSession* const GameSession = Cast<ABSGameSession>(GameMode->GameSession))
		{
			// Only update if the status has changed.
			const EOnlineAsyncTaskState::Type NewSearchStatus = GameSession->GetSearchStatus();

			// If the status changed.
			bChanged = SearchStatus != NewSearchStatus;

			if (bChanged)
			{
				SearchStatus = NewSearchStatus;

				switch (SearchStatus)
				{
				case EOnlineAsyncTaskState::Done:
					UpdateServerList(GameSession);
					break;
				case EOnlineAsyncTaskState::Failed:
					CurrentServerList.Empty();
					break;
				}
			}
		}
	}

	return bChanged;
}


bool UBSServerBrowserHost::IsSearchInProgress() const
{
	return SearchStatus == EOnlineAsyncTaskState::InProgress;
}

bool UBSServerBrowserHost::IsSearchFinished() const
{
	return SearchStatus == EOnlineAsyncTaskState::Done;
}

bool UBSServerBrowserHost::IsSearchedFailed() const
{
	return SearchStatus == EOnlineAsyncTaskState::Failed;
}

bool UBSServerBrowserHost::JoinSession(const FServerListEntry& Entry)
{
	bool bWasSuccessful = false;
	UBSGameInstance* const GameInstance = GetGameInstance();

	if (GameInstance)
	{
		const FLocalPlayerContext& PlayerContext = GetLocalPlayerContext();
		bWasSuccessful = GameInstance->JoinSession(PlayerContext.GetLocalPlayer(), Entry.SearchResult);
	}

	// #bstodo Need a way to notify UI of join failure

	return bWasSuccessful;
}

void UBSServerBrowserHost::UpdateServerList(ABSGameSession* GameSession)
{
	CurrentServerList.Empty();

	const auto& Results = GameSession->GetSearchResults();

	for (const auto& Result : Results)
	{
		FServerListEntry Entry;

		Entry.SearchResult = Result;
		Entry.Name = Result.Session.OwningUserName;
		Result.Session.SessionSettings.Get(SETTING_MAPNAME, Entry.CurrentMap);
		Result.Session.SessionSettings.Get(SETTING_GAMEMODE, Entry.GameMode);
		Entry.PingInMs = Result.PingInMs;
		Entry.MaxConnections = Result.Session.SessionSettings.NumPublicConnections;
		Entry.AvailableConnections = Result.Session.NumOpenPublicConnections;

		CurrentServerList.Push(Entry);
	}
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
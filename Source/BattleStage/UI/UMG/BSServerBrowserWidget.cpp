// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSServerBrowserWidget.h"
#include "BSGameInstance.h"
#include "Online/BSGameSession.h"




bool UBSServerBrowserWidget::SearchForSessions()
{
	bool bWasSuccessful = false;
	UBSGameInstance* const GameInstance = GetGameInstance();

	if (GameInstance)
	{
		OnStartSessionSearch();

		bWasSuccessful = GameInstance->FindSessions(GetOwningLocalPlayer(), FString{});
	}

	UpdateSearchStatus();

	return bWasSuccessful;
}

const TArray<FServerListEntry>& UBSServerBrowserWidget::GetSearchResults() const
{
	return CurrentServerList;
}

bool UBSServerBrowserWidget::UpdateSearchStatus()
{
	bool bChanged = false;

	ABSGameMode* const GameMode = Cast<ABSGameMode>(GetWorld()->GetAuthGameMode());

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

bool UBSServerBrowserWidget::IsSearchInProgress() const
{
	return SearchStatus == EOnlineAsyncTaskState::InProgress;
}

bool UBSServerBrowserWidget::IsSearchFinished() const
{
	return SearchStatus == EOnlineAsyncTaskState::Done;
}

bool UBSServerBrowserWidget::IsSearchedFailed() const
{
	return SearchStatus == EOnlineAsyncTaskState::Failed;
}

bool UBSServerBrowserWidget::JoinSession(const FServerListEntry& Entry)
{
	bool bWasSuccessful = false;
	UBSGameInstance* const GameInstance = GetGameInstance();

	if (GameInstance)
	{
		bWasSuccessful = GameInstance->JoinSession(GetOwningLocalPlayer(), Entry.SearchResult);
	}

	// #bstodo Need a way to notify UI of join failure

	return bWasSuccessful;
}

void UBSServerBrowserWidget::UpdateServerList(class ABSGameSession* GameSession)
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

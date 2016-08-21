// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSGameSession.h"

#include "Online.h"
#include "../OnlineSubsystem/Public/Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystemUtils.h"

#include "BSOnlineSessionSettings.h"

ABSGameSession::ABSGameSession(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &ABSGameSession::OnCreateDelegateComplete);
	OnFindSessionsCompletedDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &ABSGameSession::OnFindSessionsComplete);
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &ABSGameSession::OnJoinSessionComplete);
}

bool ABSGameSession::CreateSession(ULocalPlayer* LocalPlayer, const FName SessionName, const int32 MaxConnections, const bool bIsLan, const FString& GameType, const FString& MapName)
{
	UE_LOG(BattleStageOnline, Log, TEXT("Creating online session %s for %s"), *SessionName.ToString(), *LocalPlayer->GetName());

	UWorld* World = GetWorld();
	IOnlineSessionPtr SessionInt = Online::GetSessionInterface(World);
	if (SessionInt.IsValid())
	{
		FBSOnlineSessionSettings Settings(MaxConnections, bIsLan, MapName, GameType);

		OnCreateSessionCompleteHandle = SessionInt->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
		return SessionInt->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), SessionName, Settings);
	}
	else
	{
		UE_LOG(BattleStageOnline, Warning, TEXT("ABSGameSession::CreateSession could not find a valid online session interface."));
		return false;
	}
}

void ABSGameSession::OnCreateDelegateComplete(FName SessionName, bool bWasSuccessful)
{
	UWorld* World = GetWorld();
	IOnlineSessionPtr SessionInt = Online::GetSessionInterface(World);
	if (SessionInt.IsValid())
	{
		SessionInt->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteHandle);

		if (bWasSuccessful)
		{
			UE_LOG(BattleStageOnline, Log, TEXT("Online session successfully created."));
		}
		else
		{
			UE_LOG(BattleStageOnline, Log, TEXT("Online session could not be created."));
		}

		OnSessionCreated().Broadcast(SessionName, bWasSuccessful);
	}
}

bool ABSGameSession::FindSessions(ULocalPlayer* LocalPlayer)
{
	UE_LOG(BattleStageOnline, Log, TEXT("Starting to find online sessions for %s"), *LocalPlayer->GetName());

	bool bOperationSuccessful = false;

	UWorld* World = GetWorld();
	IOnlineSessionPtr SessionInt = Online::GetSessionInterface(World);
	if (SessionInt.IsValid())
	{
		OnFindSessionsCompleteHandle = SessionInt->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompletedDelegate);

		SearchSettings = MakeShareable(new FBSOnlineSessionSearch());
		bOperationSuccessful = SessionInt->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SearchSettings.ToSharedRef());
	}

	return bOperationSuccessful;
}

void ABSGameSession::OnFindSessionsComplete(bool bWasSuccessful)
{	
	UE_LOG(BattleStageOnline, Log, TEXT("Find sessions completed: %s, found %d sessions."), bWasSuccessful ? TEXT("successfully") : TEXT("failure"), bWasSuccessful ? SearchSettings->SearchResults.Num() : 0);

	UWorld* World = GetWorld();
	IOnlineSessionPtr SessionInt = Online::GetSessionInterface(World);
	if (SessionInt.IsValid())
	{
		SessionInt->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteHandle);
	}

	OnFindSessionsComplete().Broadcast(bWasSuccessful && SearchSettings->SearchResults.Num() > 0);
}

const TArray<FOnlineSessionSearchResult>& ABSGameSession::GetSearchResults() const
{
	static const TArray<FOnlineSessionSearchResult> EmptySearch;
	return SearchSettings.IsValid() ? SearchSettings->SearchResults : EmptySearch;
}

const EOnlineAsyncTaskState::Type ABSGameSession::GetSearchStatus() const
{
	EOnlineAsyncTaskState::Type Status = EOnlineAsyncTaskState::NotStarted;

	if (SearchSettings.IsValid())
	{
		Status = SearchSettings->SearchState;
	}

	return Status;
}

bool ABSGameSession::JoinSession(ULocalPlayer* LocalPlayer, const FOnlineSessionSearchResult& SearchResult)
{
	bool bOperationSuccessful = false;

	if (SearchResult.IsValid())
	{
		UE_LOG(BattleStageOnline, Log, TEXT("Starting to join online session by %s for %s"), *SearchResult.Session.OwningUserName, *LocalPlayer->GetName());

		UWorld* World = GetWorld();
		IOnlineSessionPtr SessionInt = Online::GetSessionInterface(World);
		if (SessionInt.IsValid())
		{
			OnJoinSessionCompleteHandle = SessionInt->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

			SessionInt->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), GameSessionName, SearchResult);
		}
	}
	else
	{
		UE_LOG(BattleStageOnline, Warning, TEXT("%s attempted to join online session using invalid search result."), *LocalPlayer->GetName());
	}

	return bOperationSuccessful;
}

void ABSGameSession::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(BattleStageOnline, Log, TEXT("Join session completed %s"), Result == EOnJoinSessionCompleteResult::Success ? TEXT("successfully") : TEXT("unsuccessfully"));
	
	UWorld* World = GetWorld();
	IOnlineSessionPtr SessionInt = Online::GetSessionInterface(World);
	if (SessionInt.IsValid())
	{
		SessionInt->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteHandle);
	}

	OnJoinSessionComplete().Broadcast(SessionName, Result);
}

void ABSGameSession::ReturnToMainMenuHost()
{
	Super::ReturnToMainMenuHost();
}
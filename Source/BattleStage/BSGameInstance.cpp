// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSGameInstance.h"
#include "Online/BSGameSession.h"
#include "BSTypes.h"

UBSGameInstance::UBSGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bIsOnline(true)
{
}

void UBSGameInstance::SetIsOnline(const bool IsOnline)
{
	bIsOnline = IsOnline;
}

bool UBSGameInstance::GetIsOnline() const
{
	return bIsOnline;
}

bool UBSGameInstance::HostSession(ULocalPlayer* LocalPlayer, const FString& InTravelUrl)
{
	bool bWasSuccessful = false;

	if (!bIsOnline)
	{
		// Load local game
		bWasSuccessful = GetWorld()->ServerTravel(InTravelUrl);
	}
	else if (ABSGameSession* GameSession = GetGameSession())
	{
		// Create online session

		// Store the travel url for use after session creation
		TravelURL = InTravelUrl;

		// Get session info from url
		const FString MapNamePrefix = "/Game/Maps/";
		FString MapName = InTravelUrl.RightChop(MapNamePrefix.Len());
		MapName = MapName.LeftChop(MapName.Len() - MapName.Find("?"));

		const bool bIsLan = UGameplayStatics::HasOption(InTravelUrl, TravelURLKeys::IsLanMatch);
		const FString GameType = UGameplayStatics::ParseOption(InTravelUrl, TravelURLKeys::GameType);

		// Set delegate and attempt to create the session
		OnHostSessionCreatedHandle = GameSession->OnSessionCreated().AddUObject(this, &UBSGameInstance::OnHostSessionCreated);

		bWasSuccessful = GameSession->CreateSession(LocalPlayer, GameSessionName, ABSGameSession::DEFAULT_MAX_PLAYERS, bIsLan, GameType, MapName);
	}

	return bWasSuccessful;
}

bool UBSGameInstance::JoinSession(ULocalPlayer* LocalPlayer, const FOnlineSessionSearchResult& SearchResult)
{
	bool bOperationSuccessful = false;

	if (!bIsOnline)
	{
		UE_LOG(BattleStageOnline, Warning, TEXT("Cannot join online session while game instance is in offline mode."));
	}
	else if (ABSGameSession* GameSession = GetGameSession())
	{
		OnJoinSessionCompleteHandle = GameSession->OnJoinSessionComplete().AddUObject(this, &UBSGameInstance::OnJoinSessionComplete);

		GameSession->JoinSession(LocalPlayer, SearchResult);
	}

	return bOperationSuccessful;
}

bool UBSGameInstance::JoinSession(ULocalPlayer* LocalPlayer, int32 SessionIndexInSearchResults)
{
	bool bOperationSuccessful = false;

	if (ABSGameSession* GameSession = GetGameSession())
	{
		const auto SearchResults = GameSession->GetSearchResults();
		if (SearchResults.IsValidIndex(SessionIndexInSearchResults))
		{
			bOperationSuccessful = JoinSession(LocalPlayer, SearchResults[SessionIndexInSearchResults]);
		}
	}

	return bOperationSuccessful;
}

void UBSGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (ABSGameSession* GameSession = GetGameSession())
	{
		GameSession->OnJoinSessionComplete().Remove(OnJoinSessionCompleteHandle);
		OnJoinSessionCompleteHandle.Reset();

		// #bstodo Handle other result types
		switch (Result)
		{
			case EOnJoinSessionCompleteResult::Success:
			{
				ULocalPlayer* const LocalPlayer = GetFirstGamePlayer();
				GameSession->TravelToSession(LocalPlayer->GetControllerId(), GameSessionName);
				break;
			}
		}
	}
}

void UBSGameInstance::OnHostSessionCreated(FName SessionName, bool bWasSuccessful)
{
	if (ABSGameSession* GameSession = GetGameSession())
	{
		GameSession->OnSessionCreated().Remove(OnHostSessionCreatedHandle);
		OnHostSessionCreatedHandle.Reset();

		if (bWasSuccessful)
		{
			// Travel to the desired level
			GetWorld()->ServerTravel(TravelURL);
		}
		else
		{
			// #bstodo Notify failed to create session
		}
	}
}

bool UBSGameInstance::FindSessions(ULocalPlayer* LocalPlayer, const FString& Filters)
{
	bool bOperationSuccessful = false;

	if (!bIsOnline)
	{
		UE_LOG(BattleStageOnline, Warning, TEXT("Cannot find online session while game instance is in offline mode."));
	}
	else if (ABSGameSession* GameSession = GetGameSession())
	{
		OnFindSessionsCompleteHandle = GameSession->OnFindSessionsComplete().AddUObject(this, &UBSGameInstance::OnFindSessionsComplete);
		
		bOperationSuccessful = GameSession->FindSessions(LocalPlayer);
	}

	return bOperationSuccessful;
}

void UBSGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (ABSGameSession* GameSession = GetGameSession())
	{
		GameSession->OnFindSessionsComplete().Remove(OnFindSessionsCompleteHandle);
		OnFindSessionsCompleteHandle.Reset();
	}

	if (!bWasSuccessful)
	{
		// #bstodo Notify could not find any sessions.
	}
	else
	{
		//JoinSession(GetLocalPlayerByIndex(0), 0);
	}
}

ABSGameSession* UBSGameInstance::GetGameSession() const
{
	ABSGameSession* GameSession = nullptr;
	AGameMode* GameMode = GetWorld()->GetAuthGameMode();
	
	if (GameMode)
	{
		GameSession = Cast<ABSGameSession>(GameMode->GameSession);
	}

	return GameSession;
}

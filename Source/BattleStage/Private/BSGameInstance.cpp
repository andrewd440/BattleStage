// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSGameInstance.h"

#include "Online/BSGameSession.h"
#include "BSMatchConfig.h"
#include "OnlineSubsystemUtils.h"

UBSGameInstance::UBSGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bIsOnline(true)
{
	MatchConfigClass = UBSMatchConfig::StaticClass();

	OnContinueDestroyingOnlineSessionDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UBSGameInstance::OnContinueDestroyingOnlineSession);
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
		FString MapPath;
		FString Options;
		InTravelUrl.Split(TEXT("?"), &MapPath, &Options);

		// Get map name
		const FString MapNamePrefix = "/Game/Maps/";
		FString MapName = MapPath.RightChop(MapNamePrefix.Len());

		// Add ? to options begin
		Options.InsertAt(0, TEXT("?"));

		const bool bIsLan = UGameplayStatics::HasOption(Options, TravelURLKeys::IsLanMatch);
		const FString GameType = UGameplayStatics::ParseOption(Options, TravelURLKeys::GameType);

		const int32 MaxPlayers = UGameplayStatics::GetIntOption(Options, TravelURLKeys::MaxPlayers, ABSGameSession::DEFAULT_MAX_PLAYERS);

		// Set delegate and attempt to create the session
		OnHostSessionCreatedHandle = GameSession->OnSessionCreated().AddUObject(this, &UBSGameInstance::OnHostSessionCreated);

		bWasSuccessful = GameSession->CreateSession(LocalPlayer, GameSessionName, MaxPlayers, bIsLan, GameType, MapName);
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

void UBSGameInstance::OnContinueDestroyingOnlineSession(FName, bool)
{
	IOnlineSessionPtr SessionInt = Online::GetSessionInterface(GetWorld());
	if (SessionInt.IsValid())
	{
		// Make sure all possible delegates are cleared from GracefullyDestroyOnlineSession.
		SessionInt->ClearOnCreateSessionCompleteDelegate_Handle(OnContinueDestroyingOnlineSessionHandle);
		SessionInt->ClearOnStartSessionCompleteDelegate_Handle(OnContinueDestroyingOnlineSessionHandle);
		SessionInt->ClearOnEndSessionCompleteDelegate_Handle(OnContinueDestroyingOnlineSessionHandle);
		SessionInt->ClearOnDestroySessionCompleteDelegate_Handle(OnContinueDestroyingOnlineSessionHandle);

		// Continue destruction
		GracefullyDestroyOnlineSession();
	}
}

void UBSGameInstance::GracefullyDestroyOnlineSession()
{
	IOnlineSessionPtr SessionInt = Online::GetSessionInterface(GetWorld());
	if (SessionInt.IsValid())
	{
		const EOnlineSessionState::Type SessionState = SessionInt->GetSessionState(GameSessionName);

		// Handle each possible state in the session state to ensure the the session is destroyed
		// gracefully.
		switch (SessionState)
		{
		case EOnlineSessionState::Creating:
			OnContinueDestroyingOnlineSessionHandle = SessionInt->AddOnCreateSessionCompleteDelegate_Handle(OnContinueDestroyingOnlineSessionDelegate);
			break;
		case EOnlineSessionState::Starting:
			OnContinueDestroyingOnlineSessionHandle = SessionInt->AddOnStartSessionCompleteDelegate_Handle(OnContinueDestroyingOnlineSessionDelegate);
			break;
		case EOnlineSessionState::InProgress:
			OnContinueDestroyingOnlineSessionHandle = SessionInt->AddOnEndSessionCompleteDelegate_Handle(OnContinueDestroyingOnlineSessionDelegate);
			SessionInt->EndSession(GameSessionName);
		case EOnlineSessionState::Ending:
			OnContinueDestroyingOnlineSessionHandle = SessionInt->AddOnEndSessionCompleteDelegate_Handle(OnContinueDestroyingOnlineSessionDelegate);
			break;
		case EOnlineSessionState::Ended:
		case EOnlineSessionState::Pending:
			OnContinueDestroyingOnlineSessionHandle = SessionInt->AddOnDestroySessionCompleteDelegate_Handle(OnContinueDestroyingOnlineSessionDelegate);
			SessionInt->DestroySession(GameSessionName);
			break;
		}
	}
}
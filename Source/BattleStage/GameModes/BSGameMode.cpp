// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "BattleStage.h"
#include "BSGameMode.h"
#include "Player/BSCharacter.h"
#include "Player/BSPlayerController.h"
#include "BSGameState.h"
#include "Player/BSPlayerState.h"
#include "UI/BSHUD.h"
#include "Online/BSGameSession.h"

DEFINE_LOG_CATEGORY_STATIC(BSGameMode, Warning, All);

ABSGameMode::ABSGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MinPlayers = 1;
	MaxPlayers = 8;

	TimeLimit = 15;
	ScoreGoal = 2;

	KillScore = 1;
	DeathScore = 0;

	WinningPlayer = nullptr;

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnFinder(TEXT("/Game/Blueprints/BP_BSCharacter"));
	DefaultPawnClass = PlayerPawnFinder.Class;

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerFinder(TEXT("/Game/Blueprints/BP_BSPlayerController"));
	PlayerControllerClass = PlayerControllerFinder.Class;

	static ConstructorHelpers::FClassFinder<AHUD> HUDFinder(TEXT("/Game/UI/HUD/BP_HUD"));
	HUDClass = HUDFinder.Class;

	PlayerStateClass = ABSPlayerState::StaticClass();
	GameStateClass = ABSGameState::StaticClass();
}

void ABSGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	TimeLimit = FMath::Max(0, UGameplayStatics::GetIntOption(Options, TEXT("TimeLimit"), TimeLimit));
	TimeLimit *= 60; // Convert minutes to seconds

	ScoreGoal = FMath::Max(0, UGameplayStatics::GetIntOption(Options, TEXT("ScoreGoal"), ScoreGoal));
}

void ABSGameMode::InitGameState()
{
	Super::InitGameState();
	
	BSGameState = Cast<ABSGameState>(GameState);

	if (BSGameState)
	{
		BSGameState->SetTimeLimit(TimeLimit);
		BSGameState->SetGoalScore(ScoreGoal);
		BSGameState->SetIsTeamGame(bIsTeamGame);
	}
	else
	{
		UE_LOG(BSGameMode, Warning, TEXT("ABSGameMode::InitGameState does not have a valid ABSGameState object. " 
			"GameState could not be initialized."))
	}
}

bool ABSGameMode::ReadyToStartMatch_Implementation()
{
	bool bIsReady = Super::ReadyToStartMatch_Implementation();

	if (NumPlayers < MinPlayers)
	{
		bIsReady = false;
	}

	return bIsReady;
}

TSubclassOf<class AGameSession> ABSGameMode::GetGameSessionClass() const
{
	return ABSGameSession::StaticClass();
}

void ABSGameMode::ScoreKill_Implementation(AController* Player, AController* Killed)
{
	ABSPlayerState* PlayerState = Cast<ABSPlayerState>(Player->PlayerState);

	if (PlayerState)
	{
		if (Player == Killed)
		{
			// Suicide
			PlayerState->ScoreDeath(PlayerState, DeathScore);
		}
		else if (Killed)
		{
			if (ABSPlayerState* KilledState = Cast<ABSPlayerState>(Killed->PlayerState))
			{
				PlayerState->ScoreKill(KilledState, KillScore);
				KilledState->ScoreDeath(PlayerState, DeathScore);
			}
		}
	}

	CheckScore(PlayerState);
}

void ABSGameMode::ScoreDeath_Implementation(AController* Player)
{
	if(ABSPlayerState* PlayerState = Cast<ABSPlayerState>(Player->PlayerState))
	{
		PlayerState->ScoreDeath(nullptr, DeathScore);
	}
}

void ABSGameMode::CheckScore(ABSPlayerState* Player)
{
	if (!bIsTeamGame)
	{
		if (!WinningPlayer && Player->Score >= ScoreGoal)
		{
			WinningPlayer = Player;
		}
	}
	else
	{
		// Get team score for Player and compare to goal
	}
}

void ABSGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	ReturnToMainMenuHost();
}

FString ABSGameMode::InitNewPlayer(class APlayerController* NewPlayerController, const TSharedPtr<const FUniqueNetId>& UniqueId, const FString& Options, const FString& Portal /*= TEXT("")*/)
{ 
	FString ReturnString = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	//if (!bIsTeamGame)
	//{
	//	if (ABSPlayerState* PlayerState = Cast<ABSPlayerState>(NewPlayerController->PlayerState))
	//	{
	//		PlayerState->SetTeam(BSGameState-);
	//	}
	//}

	return ReturnString;
}

bool ABSGameMode::ReadyToEndMatch_Implementation()
{
	return WinningPlayer != nullptr || BSGameState->GetRemainingTime() <= 0;
}
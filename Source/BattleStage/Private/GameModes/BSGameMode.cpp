// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "BattleStage.h"
#include "BSGameMode.h"

#include "BSCharacter.h"
#include "BSPlayerController.h"
#include "BSGameState.h"
#include "BSPlayerState.h"
#include "BSHUD.h"
#include "BSGameSession.h"
#include "BSTypes.h"

DEFINE_LOG_CATEGORY_STATIC(BSGameMode, Warning, All);

ABSGameMode::ABSGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bPauseable = false;

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

	MinPlayers = UGameplayStatics::GetIntOption(Options, TravelURLKeys::MinPlayers, MinPlayers);
	MaxPlayers = UGameplayStatics::GetIntOption(Options, TravelURLKeys::MaxPlayers, MaxPlayers);

	TimeLimit = FMath::Max(0, UGameplayStatics::GetIntOption(Options, TravelURLKeys::TimeLimit, TimeLimit));
	TimeLimit *= 60; // Convert minutes to seconds

	ScoreGoal = FMath::Max(0, UGameplayStatics::GetIntOption(Options, TravelURLKeys::ScoreGoal, ScoreGoal));
}

void ABSGameMode::InitGameState()
{
	Super::InitGameState();
	
	BSGameState = Cast<ABSGameState>(GameState);

	if (BSGameState)
	{
		BSGameState->TimeLimit = TimeLimit;
		BSGameState->ScoreGoal = ScoreGoal;
		BSGameState->bIsTeamGame = bIsTeamGame;
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

void ABSGameMode::ScoreKill_Implementation(AController* Scorer, AController* Killed)
{
	ABSPlayerState* ScorerState = Cast<ABSPlayerState>(Scorer->PlayerState);

	if (ScorerState)
	{
		if (Scorer == Killed)
		{
			// Suicide
			ScorerState->ScoreDeath(ScorerState, DeathScore);
			
			BSGameState->AddScore(ScorerState, nullptr, DeathScore, EScoreType::Suicide);
		}
		else if (Killed)
		{			
			if (ABSPlayerState* KilledState = Cast<ABSPlayerState>(Killed->PlayerState))
			{
				ScorerState->ScoreKill(KilledState, KillScore);
				KilledState->ScoreDeath(ScorerState, DeathScore);

				BSGameState->AddScore(ScorerState, KilledState, KillScore, EScoreType::Kill);
			}
		}
	}

	CheckScore(ScorerState);
}

void ABSGameMode::ScoreDeath_Implementation(AController* Scorer)
{
	if(ABSPlayerState* ScorerState = Cast<ABSPlayerState>(Scorer->PlayerState))
	{
		ScorerState->ScoreDeath(nullptr, DeathScore);

		BSGameState->AddScore(ScorerState, nullptr, DeathScore, EScoreType::Death);
	}
}

void ABSGameMode::HostTerminateGame()
{
	EndMatch();

	GameSession->ReturnToMainMenuHost();
}

TSubclassOf<UBSScoreboardWidget> ABSGameMode::GetScoreboardWidget() const
{
	return ScoreboardWidget;
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

	// #bstodo If no winning player, set highest scoring player

	for (FConstControllerIterator ItrController = GetWorld()->GetControllerIterator(); ItrController; ++ItrController)
	{
		AController* const Controller = *ItrController;
		Controller->GameHasEnded(nullptr, Controller->PlayerState == WinningPlayer);
	}

	FTimerHandle RestartHandle;
	GetWorld()->GetTimerManager().SetTimer(RestartHandle, this, &ABSGameMode::RestartGame, 10.f, false);
}

bool ABSGameMode::ReadyToEndMatch_Implementation()
{
	return WinningPlayer != nullptr || BSGameState->GetRemainingTime() <= 0;
}
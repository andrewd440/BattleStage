// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSGameState.h"

DEFINE_LOG_CATEGORY_STATIC(ABSGameState, Warning, All);

void ABSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABSGameState, TimeLimit, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ABSGameState, ScoreGoal, COND_InitialOnly);

	DOREPLIFETIME(ABSGameState, LastScoreEvent);
}

void ABSGameState::AddScore(ABSPlayerState* Scorer, ABSPlayerState* Victim, const int32 Score, const EScoreType ScoreType)
{
	LastScoreEvent.Type = ScoreType;
	LastScoreEvent.Scorer = Scorer;
	LastScoreEvent.ScorerPoints = Score;
	LastScoreEvent.Victim = Victim;

	LastScoreEvent.ForceReplication();

	OnRecievedScoreEvent();
}

void ABSGameState::OnRecievedScoreEvent()
{
	OnScoreEvent().Broadcast(LastScoreEvent);
}

void ABSGameState::AddPlayerState(class APlayerState* PlayerState)
{		
	Super::AddPlayerState(PlayerState);

	if (!PlayerState->bIsInactive)
	{
		OnPlayerJoinLeave().Broadcast(PlayerState, true);
	}
}

void ABSGameState::RemovePlayerState(class APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);

	OnPlayerJoinLeave().Broadcast(PlayerState, false);
}

void ABSGameState::QuitGameAndReturnToMainMenu()
{
	if (AuthorityGameMode)
	{
		// This is the server, end the game
		if (ABSGameMode* const GameMode = Cast<ABSGameMode>(AuthorityGameMode))
		{
			GameMode->HostTerminateGame();
		}
	}
	else if(APlayerController* const PlayController = GetGameInstance()->GetFirstLocalPlayerController())
	{
		// Client, leave game
		PlayController->ClientReturnToMainMenu(FString{});
	}
}
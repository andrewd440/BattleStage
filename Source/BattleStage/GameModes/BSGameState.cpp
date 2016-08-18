// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSGameState.h"
#include "UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(ABSGameState, Warning, All);

void ABSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABSGameState, TimeLimit, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ABSGameState, GoalScore, COND_InitialOnly);
}

void ABSGameState::DefaultTimer()
{
	Super::DefaultTimer();

	OnRemainingTimeChanged.Broadcast();
}

void ABSGameState::AddScore(ABSPlayerState* Scorer, const int32 Score)
{

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

void ABSGameState::SetTimeLimit(const int32 Time)
{
	TimeLimit = Time;
}
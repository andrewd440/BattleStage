// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "BattleStage.h"
#include "BSGameMode.h"
#include "BSCharacter.h"
#include "BSPlayerController.h"
#include "BSGameState.h"

DEFINE_LOG_CATEGORY_STATIC(BSGameMode, Warning, All);

ABSGameMode::ABSGameMode()
	: Super()
{
	TimeLimit = 15;
	ScoreGoal = 20;
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
	}
	else
	{
		UE_LOG(BSGameMode, Warning, TEXT("ABSGameMode::InitGameState does not have a valid ABSGameState object. " 
			"GameState could not be initialized."))
	}
}

void ABSGameMode::ScoreKill(ABSPlayerState* Player, ABSPlayerState* Killed)
{

}

void ABSGameMode::ScoreDeath(ABSPlayerState* Player)
{

}

void ABSGameMode::CheckScore(ABSPlayerState* Player)
{

}

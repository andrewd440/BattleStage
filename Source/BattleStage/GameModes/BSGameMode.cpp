// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "BattleStage.h"
#include "BSGameMode.h"
#include "BSCharacter.h"
#include "BSPlayerController.h"
#include "BSGameState.h"
#include "BSPlayerState.h"

DEFINE_LOG_CATEGORY_STATIC(BSGameMode, Warning, All);

ABSGameMode::ABSGameMode()
	: Super()
{
	TimeLimit = 15;
	ScoreGoal = 20;

	KillScore = 1;
	DeathScore = 0;
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

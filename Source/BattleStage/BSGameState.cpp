// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSGameState.h"
#include "UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(ABSGameState, Warning, All);

void ABSGameState::SetTimeLimit(const int32 Time)
{
	TimeLimit = Time;
	ReplicatedTimeRemaining = Time;
	TimeRemaining = Time;
}

void ABSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABSGameState, TimeLimit, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ABSGameState, GoalScore, COND_InitialOnly);
	DOREPLIFETIME(ABSGameState, ReplicatedTimeRemaining);
	//DOREPLIFETIME(ABSGameState, TeamScores);
}

void ABSGameState::DefaultTimer()
{
	Super::DefaultTimer();

	TimeRemaining--;

	if (GetNetMode() != NM_Client)
	{
		// Replicate time remaining on 10 sec intervals
		const int32 TimeRepInterval = 10;
		if (TimeRemaining % TimeRepInterval == 0)
		{
			ReplicatedTimeRemaining = TimeRemaining;
		}
	}

	OnRemainingTimeChanged.Broadcast();
}

void ABSGameState::AddScore(ABSPlayerState* Scorer, const int32 Score)
{

}

//int32 ABSGameState::GetTeamScore(const int32 Team)
//{
//	if (TeamScores.Num() <= Team)
//	{
//		return TeamScores[Team];
//	}
//	
//	UE_LOG(ABSGameState, Warning, TEXT("ABSGameState::GetTeamScore trying to get score for player with invalid team."));
//	return -1;
//}
//
//int32 ABSGameState::GetTeamScoreByPosition(const int32 Position, const int32 Excluded /*= -1*/)
//{
//	auto SortedScores = TeamScores;
//
//	if (Excluded >= 0)
//	{
//		SortedScores.RemoveAt(Excluded);
//	}
//
//	SortedScores.Sort(); // Sorted lowest -> highest
//	return SortedScores[SortedScores.Num() - 1 - Position];
//}

//void ABSGameState::AddPlayerState(class APlayerState* PlayerState)
//{
//	if(PlayerState)
//}

void ABSGameState::OnRep_ReplicatedTimeRemaining()
{
	TimeRemaining = ReplicatedTimeRemaining;
}

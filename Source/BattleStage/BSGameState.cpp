// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSGameState.h"
#include "UnrealNetwork.h"

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

void ABSGameState::OnRep_ReplicatedTimeRemaining()
{
	TimeRemaining = ReplicatedTimeRemaining;
}

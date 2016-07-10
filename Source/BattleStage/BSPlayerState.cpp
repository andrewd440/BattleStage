// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSPlayerState.h"
#include "UnrealNetwork.h"


void ABSPlayerState::ScoreKill(ABSPlayerState* Killed, int32 Points)
{
	Score += Points;
}

void ABSPlayerState::ScoreDeath(ABSPlayerState* Killer, int32 Points)
{
	Score += Points;
}

void ABSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABSPlayerState, Kills);
	DOREPLIFETIME(ABSPlayerState, Deaths);
}

void ABSPlayerState::SetTeam(int32 Team)
{
	CurrentTeam = Team;
}

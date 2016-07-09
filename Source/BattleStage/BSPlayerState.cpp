// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSPlayerState.h"
#include "UnrealNetwork.h"


void ABSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABSPlayerState, Kills);
	DOREPLIFETIME(ABSPlayerState, Deaths);
}

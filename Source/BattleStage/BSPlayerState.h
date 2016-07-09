// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "BSPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESTAGE_API ABSPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	void ScoreKill();

	void ScoreDeath();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = PlayerState)
	int32 Kills = 0;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = PlayerState)
	int32 Deaths = 0;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = PlayerState)
	int32 CurrentTeam = 0;
};

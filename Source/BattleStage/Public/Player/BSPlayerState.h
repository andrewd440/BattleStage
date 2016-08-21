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
	void ScoreKill(ABSPlayerState* Killed, int32 Points);

	void ScoreDeath(ABSPlayerState* Killer, int32 Points);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void SetTeam(int32 Team);

protected:
	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = PlayerState)
	int32 Kills = 0;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = PlayerState)
	int32 Deaths = 0;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = PlayerState)
	int32 CurrentTeam = 0;

public:
	int32 GetKillCount() const { return Kills; }
	int32 GetDeathCount() const { return Deaths; }
	int32 GetCurrentTeam() const { return CurrentTeam; }
};

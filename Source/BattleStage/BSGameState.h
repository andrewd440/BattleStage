// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "BSGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRemainingTimeChangedEvent);

/**
 * 
 */
UCLASS()
class BATTLESTAGE_API ABSGameState : public AGameState
{
	GENERATED_BODY()

public:
	/** Get the time remaining it this match */
	UFUNCTION(BlueprintCallable, Category = GameState)
	int32 GetRemainingTime() const { return TimeRemaining; }
	
	/** Set the time limit for the current match */
	void SetTimeLimit(const int32 Time);
	
	/** Get the time limit for the current match */
	int32 GetTimeLimit() const { return TimeLimit; }

	void SetGoalScore(int32 Score) { GoalScore = Score; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void DefaultTimer() override;

public:
	// Broadcasted when the remaining time for the match is updated.
	UPROPERTY(BlueprintAssignable, Category = GameState)
	FOnRemainingTimeChangedEvent OnRemainingTimeChanged;

private:
	UFUNCTION()
	void OnRep_ReplicatedTimeRemaining();

private:
	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = GameState, meta = (AllowPrivateAccess = "true"))
	int32 TimeLimit;

	// Time remaining that is replicated at a specific interval
	UPROPERTY(Transient, ReplicatedUsing = OnRep_ReplicatedTimeRemaining)
	int32 ReplicatedTimeRemaining;

	// The running time remaining in the match
	UPROPERTY(Transient, BlueprintReadOnly, Category = GameState, meta = (AllowPrivateAccess = "true"))
	int32 TimeRemaining;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = GameState, meta = (AllowPrivateAccess = "true"))
	int32 GoalScore;

	TArray<int32> TeamScores;
};

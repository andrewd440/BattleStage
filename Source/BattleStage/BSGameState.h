// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "BSGameState.generated.h"

class ABSPlayerState;

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

	/**
	* Add to a teams' current score.
	*
	* @param Team	The team to add the score to.
	* @param Score	The score to add.
	*/
	UFUNCTION(BlueprintCallable, Category = GameState)
	void AddScore(ABSPlayerState* Scorer, const int32 Score);

	///**
	//* Get a teams' current score.
	//*
	//* @param Team	The team to get the score for.
	//*/
	//UFUNCTION(BlueprintCallable, Category = GameState)
	//int32 GetTeamScore(const int32 Team);

	///**
	//* Get the score for a team based on the current game ranking.
	//*
	//* @param Position	The position of the team to get the score for.
	//* @param Excluded	Optional team to exclude from the ranking position.
	//*/
	//UFUNCTION(BlueprintCallable, Category = GameState)
	//int32 GetTeamScoreByPosition(const int32 Position, const int32 Excluded = -1);

	UFUNCTION(BlueprintCallable, Category = GameState)
	bool IsTeamGame() const { return bIsTeamGame; }

	void SetIsTeamGame(uint32 IsTeamGame) { bIsTeamGame = IsTeamGame; }

	//virtual void AddPlayerState(class APlayerState* PlayerState) override;

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

	//UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = GameState, meta = (AllowPrivateAccess = "true"))
	//TArray<int32> TeamScores;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = GameState, meta = (AllowPrivateAccess = "true"))
	uint32 bIsTeamGame : 1;
};

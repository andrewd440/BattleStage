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
	int32 GetRemainingTime() const { return TimeLimit - ElapsedTime; }
	
	/** Set the time limit for the current match */
	void SetTimeLimit(const int32 Time);
	
	/** Get the time limit for the current match */
	int32 GetTimeLimit() const { return TimeLimit; }

	void SetGoalScore(int32 Score) { GoalScore = Score; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void DefaultTimer() override;

	/**
	* Add a score that was created by a player.
	*
	* @param Scorer	The player state the scored.
	* @param Score	The score to add.
	*/
	UFUNCTION(BlueprintCallable, Category = GameState)
	void AddScore(ABSPlayerState* Scorer, const int32 Score);

	UFUNCTION(BlueprintCallable, Category = GameState)
	bool IsTeamGame() const { return bIsTeamGame; }

	void SetIsTeamGame(uint32 IsTeamGame) { bIsTeamGame = IsTeamGame; }

public:
	// Broadcasted when the remaining time for the match is updated.
	UPROPERTY(BlueprintAssignable, Category = GameState)
	FOnRemainingTimeChangedEvent OnRemainingTimeChanged;

private:
	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = GameState, meta = (AllowPrivateAccess = "true"))
	int32 TimeLimit;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = GameState, meta = (AllowPrivateAccess = "true"))
	int32 GoalScore;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = GameState, meta = (AllowPrivateAccess = "true"))
	uint32 bIsTeamGame : 1;
};

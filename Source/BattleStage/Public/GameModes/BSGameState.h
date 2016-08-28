// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "BSGameState.generated.h"

class ABSPlayerState;

UENUM()
enum class EScoreType : uint8
{
	Kill,
	Death,
	Suicide,
};

USTRUCT()
struct FScoreEvent
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	EScoreType Type;

	UPROPERTY()
	TWeakObjectPtr<ABSPlayerState> Scorer;

	UPROPERTY()
	uint8 ScorerPoints; // Points rewarded to the scorer

	UPROPERTY()
	TWeakObjectPtr<ABSPlayerState> Victim;

	/** Ensures the score event is replicated. */
	void ForceReplication()
	{
		++ReplicationByte;
	}

private:
	UPROPERTY()
	uint8 ReplicationByte;
};

/**
 * 
 */
UCLASS()
class BATTLESTAGE_API ABSGameState : public AGameState
{
	GENERATED_BODY()

public:
	/** Broadcasted when a score event has been received */
	DECLARE_EVENT_OneParam(ABSGameState, FOnScoreEvent, const FScoreEvent&);
	FOnScoreEvent& OnScoreEvent() { return OnScoreEventReceived; }

	/** Broadcasted when a player joins or leaves the game */
	DECLARE_EVENT_TwoParams(ABSGameState, FOnPlayerJoinLeaveEvent, APlayerState* /*PlayerState*/, bool /*bIsJoin*/);
	FOnPlayerJoinLeaveEvent& OnPlayerJoinLeave() { return OnPlayerJoinLeaveEvent; }

public:
	/** Get the time remaining it this match */
	UFUNCTION(BlueprintCallable, Category = GameState)
	int32 GetRemainingTime() const { return TimeLimit - ElapsedTime; }
	
	/** Get the time limit for the current match */
	int32 GetTimeLimit() const { return TimeLimit; }

	/** Get the score goal for the current game */
	int32 GetScoreGoal() const { return ScoreGoal; }

	UFUNCTION(BlueprintCallable, Category = GameState)
	bool IsTeamGame() const { return bIsTeamGame; }
	
	/**
	* Called by local players to quit the current game and return to the main menu. 
	* If this is called by the host of the game, the game will be ended and connected clients will be
	* disconnected from the game and returned to the main menu. 
	*/
	void QuitGameAndReturnToMainMenu();

	/** AGameState Interface Begin */
	virtual void AddPlayerState(class APlayerState* PlayerState) override;
	virtual void RemovePlayerState(class APlayerState* PlayerState) override;
	/** AGameState Interface End */

	/** AActor Interface Begin */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	/** AActor Interface End */

protected:
	/**
	* Add a score that was created by a player.
	*
	* @param Scorer		The player state the scored.
	* @param Victim		The player that was a victim of the scorer. (i.e. was killed)
	* @param Score		The score to add.
	* @param ScoreType	The type of score event.
	*/
	UFUNCTION(BlueprintCallable, Category = GameState)
	void AddScore(ABSPlayerState* Scorer, ABSPlayerState* Victim, const int32 Score, const EScoreType ScoreType);

	/**
	 * Called when a score event has been received.
	 * 
	 * Derived implementations should always call base implementation to broadcast
	 * score event.
	 */
	UFUNCTION()
	virtual void OnRecievedScoreEvent();

protected:
	friend class ABSGameMode;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = GameState, meta = (AllowPrivateAccess = "true"))
	int32 TimeLimit;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = GameState, meta = (AllowPrivateAccess = "true"))
	int32 ScoreGoal;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = GameState, meta = (AllowPrivateAccess = "true"))
	uint32 bIsTeamGame : 1;

	/** The last score event that was received */
	UPROPERTY(ReplicatedUsing = OnRecievedScoreEvent)
	FScoreEvent LastScoreEvent;

	/** Event broadcasted when a score event is received */
	FOnScoreEvent OnScoreEventReceived;

	FOnPlayerJoinLeaveEvent OnPlayerJoinLeaveEvent;
};

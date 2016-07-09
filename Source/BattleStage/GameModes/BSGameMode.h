// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "BSGameMode.generated.h"

class ABSPlayerState;

UCLASS(minimalapi)
class ABSGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABSGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void InitGameState() override;

	/**
	* Score a player kill.
	*
	* @param Player	The player that got the kill.
	* @param Killed	The player that was killed.
	*/
	void ScoreKill(ABSPlayerState* Player, ABSPlayerState* Killed);
	
	/**
	* Score a player death that wasn't directly caused by
	* another player.
	*/
	void ScoreDeath(ABSPlayerState* Player);

protected:

	/**
	* Checks the games score for game specific conditions.
	* This is called following any type of scoring event.
	* 
	* @param Player	The player that created the score event.
	*/
	virtual void CheckScore(ABSPlayerState* Player);

protected:

	// Score goal for the game
	UPROPERTY(config, EditDefaultsOnly, Category = GameMode)
	int32 ScoreGoal;

	// Minutes the game will last
	UPROPERTY(config, EditDefaultsOnly, Category = GameMode)
	int32 TimeLimit;	

	// Score for each kill
	UPROPERTY(config, EditDefaultsOnly, Category = GameMode)
	int32 KillScore;

	// Score for each death
	UPROPERTY(config, EditDefaultsOnly, Category = GameMode)
	int32 DeathScore;

private:
	// Cast of GameState to BSGameState
	class ABSGameState* BSGameState;
};




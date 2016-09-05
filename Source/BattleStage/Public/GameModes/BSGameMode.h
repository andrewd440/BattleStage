// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "BSGameMode.generated.h"

class ABSPlayerState;

// #bstodo Break this out into 2 derived types to handle scoring: TeamGameMode and NonTeamGameMode

UCLASS(minimalapi)
class ABSGameMode : public AGameMode
{
	GENERATED_UCLASS_BODY()

public:
	/**
	* Score a player kill.
	*
	* @param Player	The player that got the kill.
	* @param Killed	The player that was killed.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = GameMode)
	void ScoreKill(AController* Scorer, AController* Killed);
	
	/**
	* Score a player death that wasn't directly caused by
	* another player.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = GameMode)
	void ScoreDeath(AController* Scorer);

	/**
	 * Called when the host has left the game and the game should be 
	 * terminated. All players will be returned to the main menu.
	 */
	void HostTerminateGame();

	/** Gets the scoreboard type used for this gamemode */
	TSubclassOf<class UBSScoreboardWidget> GetScoreboardWidget() const;	

protected:

	/**
	* Checks the games score for game specific conditions.
	* This is called following any type of scoring event.
	* 
	* @param Player	The player that created the score event.
	*/
	virtual void CheckScore(ABSPlayerState* Player);

	/** AGameMode Interface Begin */
public:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() override;
	virtual TSubclassOf<class AGameSession> GetGameSessionClass() const override;
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;

protected:
	virtual bool ReadyToStartMatch_Implementation() override;
	virtual bool ReadyToEndMatch_Implementation() override;
	virtual void HandleMatchHasEnded() override;
	/** AGameMode Interface End */

protected:
	UPROPERTY(config, EditDefaultsOnly, Category = GameMode)
	int32 MinPlayers;

	UPROPERTY(config, EditDefaultsOnly, Category = GameMode)
	int32 MaxPlayers;

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

	UPROPERTY(EditDefaultsOnly, Category = GameMode)
	uint32 bIsTeamGame : 1;

	UPROPERTY(EditDefaultsOnly, Category = GameMode)
	TSubclassOf<class UBSScoreboardWidget> ScoreboardWidget;

	// Will be assigned the match winner at the end of a non-team based game.
	ABSPlayerState* WinningPlayer;

private:
	// Cached cast of GameState to BSGameState
	class ABSGameState* BSGameState;
};




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
	UFUNCTION(BlueprintNativeEvent, Category = GameMode)
	void ScoreKill(AController* Player, AController* Killed);
	
	/**
	* Score a player death that wasn't directly caused by
	* another player.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = GameMode)
	void ScoreDeath(AController* Player);

	TSubclassOf<class AGameSession> GetGameSessionClass() const override;

protected:

	/**
	* Checks the games score for game specific conditions.
	* This is called following any type of scoring event.
	* 
	* @param Player	The player that created the score event.
	*/
	virtual void CheckScore(ABSPlayerState* Player);


	virtual FString InitNewPlayer(class APlayerController* NewPlayerController, const TSharedPtr<const FUniqueNetId>& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;

	bool ReadyToEndMatch_Implementation() override;

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

	UPROPERTY(EditDefaultsOnly, Category = GameMode)
	uint32 bIsTeamGame : 1;

	// Will be assigned the match winner at the end of a non-team based game.
	ABSPlayerState* WinningPlayer;

private:
	// Cached cast of GameState to BSGameState
	class ABSGameState* BSGameState;
};




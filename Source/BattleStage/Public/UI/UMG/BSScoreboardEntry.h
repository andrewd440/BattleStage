// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "BSScoreboardEntry.generated.h"

class UTextBlock;

class ABSPlayerState;

/**
* The type of widget that is used to display an entry into the scoreboard
* widget for a specific player.
*
* Scoreboard entries can be extended for derived BSScoreboardWidgets by specifying
* more data with required property bindings for widgets that display the information.
*/
UCLASS(Abstract)
class BATTLESTAGE_API UBSScoreboardEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	UBSScoreboardEntry(const FObjectInitializer& ObjectInitalizer = FObjectInitializer::Get());

	TWeakObjectPtr<const ABSPlayerState> GetPlayerState() const;

	/**
	 * Set the player state used to generate data for the entry. Sets any widget
	 * elements to reflect data in the new PlayerState.
	 */
	virtual void SetPlayerState(const ABSPlayerState* PlayerState);

private:
	//-----------------------------------------------------------------
	// Text getters
	//-----------------------------------------------------------------
	UFUNCTION()
	FText GetKillsText();
	UFUNCTION()
	FText GetDeathsText();
	UFUNCTION()
	FText GetPingText();

protected:
	UPROPERTY(BlueprintReadOnly, Category = ScoreboardEntry)
	TWeakObjectPtr<const ABSPlayerState> PlayerState;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KillsText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DeathsText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PingText;
};

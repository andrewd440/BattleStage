// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HUD/BSHUDWidget.h"
#include "BSScoreboardWidget.generated.h"

struct FScoreEvent;
class ABSGameState;

/**
 * Represents the in-game scoreboard for the ongoing game.
 * 
 * #bstodo Respond to player enter/leave game while scoreboard is up. Currently
 *			does not add/remove the player from the scoreboard until reopened.
 */
UCLASS(Abstract)
class BATTLESTAGE_API UBSScoreboardWidget : public UBSUserWidget
{
	GENERATED_BODY()
	
public:
	UBSScoreboardWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void InitializeScoreboard(const ABSGameState& GameState);

	/** UUserWidget Interface Begin */
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	/** UUserWidget Interface End */
	
protected:
	/** Called when a player state is joining or leaving the game. */
	virtual void OnPlayerJoinLeaveGame(APlayerState* PlayerState, bool bIsJoin);

protected:
	/** Container for all scoreboard entries */
	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ScoreboardEntryPanel;

	UPROPERTY(EditDefaultsOnly, Category = Scoreboard)
	TSubclassOf<class UBSScoreboardEntry> ScoreboardEntryClass;

private:
	/** Local list of all current scoreboard entries */
	TArray<UBSScoreboardEntry*> ScoreboardEntries;

	FDelegateHandle OnPlayJoinLeaveHandle;
};

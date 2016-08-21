// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BSHUDWidget.h"
#include "BSHUDWidget_GameClock.generated.h"

class UTextBlock;
class UProgressBar;

/**
 * 
 */
UCLASS(Abstract)
class BATTLESTAGE_API UBSHUDWidget_GameClock : public UBSHUDWidget
{
	GENERATED_BODY()
	
protected:
	/** UUserWidget Interface Begin */
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	/** UUserWidget Interface End */

private:
	void UpdateGameTimer();
	void UpdateTeamScore(const int32 ScoreGoal);
	void UpdateEnemyScore(const int32 ScoreGoal);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TeamScoreText;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* TeamScoreMeter;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* EnemyScoreText;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* EnemyScoreMeter;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GameTimerMinutes;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GameTimerSeconds;

	int32 RemainingGameTime; // Cached remaining time that is reflected in UI elements
	int32 TeamScore; // Cached enemy score that is reflected in UI elements
	int32 EnemyScore; // Cached enemy score that is reflected in UI elements

	TWeakObjectPtr<class ABSGameState> BSGameState;
};

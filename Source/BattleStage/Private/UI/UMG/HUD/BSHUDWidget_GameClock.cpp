// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSHUDWidget_GameClock.h"

void UBSHUDWidget_GameClock::NativeConstruct()
{
	Super::NativeConstruct();

	RemainingGameTime = 0;
	TeamScore = 0;
	EnemyScore = 0;

	BSGameState = GetWorld()->GetGameState<ABSGameState>();
}

void UBSHUDWidget_GameClock::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (BSGameState.IsValid())
	{
		UpdateGameTimer();

		const int32 ScoreGoal = BSGameState->GetScoreGoal();
		UpdateTeamScore(ScoreGoal);
		UpdateEnemyScore(ScoreGoal);
	}
}

void UBSHUDWidget_GameClock::UpdateGameTimer()
{
	// Set new timer text if time changed
	const int32 NewRemainingTime = BSGameState->GetRemainingTime();
	if (RemainingGameTime != NewRemainingTime)
	{
		const static FNumberFormattingOptions NumberFormat = FNumberFormattingOptions().SetMinimumIntegralDigits(2);

		const int32 NewSeconds = NewRemainingTime % 60;
		if (NewSeconds == 59)
		{
			// Change minutes text
			GameTimerMinutes->SetText(FText::AsNumber(NewRemainingTime / 60, &NumberFormat));
		}

		// Change seconds text
		GameTimerSeconds->SetText(FText::AsNumber(NewSeconds, &NumberFormat));
		RemainingGameTime = NewRemainingTime;
	}
}

void UBSHUDWidget_GameClock::UpdateTeamScore(const int32 ScoreGoal)
{
	int32 NewTeamScore = 0;

	if (!BSGameState->IsTeamGame())
	{
		// Use our score
		if (const APlayerState* const PlayerState = GetOwningPlayer()->PlayerState)
		{
			NewTeamScore = PlayerState->Score;
		}
	}

	if (TeamScore != NewTeamScore)
	{
		TeamScoreText->SetText(FText::AsNumber(NewTeamScore));
		TeamScoreMeter->SetPercent(NewTeamScore / (float)ScoreGoal);
		TeamScore = NewTeamScore;
	}
}

void UBSHUDWidget_GameClock::UpdateEnemyScore(const int32 ScoreGoal)
{
	int32 NewEnemyScore = 0;

	if (!BSGameState->IsTeamGame())
	{
		const APlayerState* const MyPlayerState = GetOwningPlayer()->PlayerState;

		// Get the max player score excluding our player
		for (const APlayerState* Player : BSGameState->PlayerArray)
		{
			if (Player != MyPlayerState  &&
				NewEnemyScore <= Player->Score)
			{
				NewEnemyScore = Player->Score;
			}
		}
	}

	if (EnemyScore != NewEnemyScore)
	{
		EnemyScoreText->SetText(FText::AsNumber(NewEnemyScore));
		EnemyScoreMeter->SetPercent(NewEnemyScore / (float)ScoreGoal);
		EnemyScore = NewEnemyScore;
	}
}

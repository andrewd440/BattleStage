// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSScoreboardEntry.h"

#include "BSPlayerState.h"

UBSScoreboardEntry::UBSScoreboardEntry(const FObjectInitializer& ObjectInitalizer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitalizer)
{
	Visibility = ESlateVisibility::HitTestInvisible;
	bIsFocusable = false;
	bCanEverTick = false;
}

TWeakObjectPtr<const ABSPlayerState> UBSScoreboardEntry::GetPlayerState() const
{
	return PlayerState;
}

void UBSScoreboardEntry::SetPlayerState(const ABSPlayerState* InPlayerState)
{
	PlayerState = InPlayerState;

	if (InPlayerState)
	{
		PlayerName->SetText(FText::FromString(InPlayerState->PlayerName));

		KillsText->TextDelegate.BindDynamic(this, &UBSScoreboardEntry::GetKillsText);
		DeathsText->TextDelegate.BindDynamic(this, &UBSScoreboardEntry::GetDeathsText);
		PingText->TextDelegate.BindDynamic(this, &UBSScoreboardEntry::GetPingText);
	}
}

FText UBSScoreboardEntry::GetKillsText()
{
	return FText::AsNumber(PlayerState->GetKillCount());
}

FText UBSScoreboardEntry::GetDeathsText()
{
	return FText::AsNumber(PlayerState->GetDeathCount());
}

FText UBSScoreboardEntry::GetPingText()
{
	return FText::AsNumber(PlayerState->Ping);
}
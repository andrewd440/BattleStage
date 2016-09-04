// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSHostGameWidget.h"

bool UBSHostGameWidget::HostGame(const FString& MatchOptions)
{
	UBSGameInstance* GameInstance = GetGameInstance();

	bool bSuccess = false;
	if (GameInstance)
	{
		bSuccess = GameInstance->HostSession(GetOwningLocalPlayer(), MatchOptions);
	}

	return bSuccess;
}

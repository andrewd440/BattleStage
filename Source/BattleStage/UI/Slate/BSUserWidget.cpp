// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSUserWidget.h"
#include "BSGameInstance.h"


UBSGameInstance* UBSUserWidget::GetGameInstance() const
{
	UBSGameInstance* GameInstance = nullptr;

	if (ULocalPlayer* LocalPlayer = GetOwningLocalPlayer())
	{
		GameInstance = Cast<UBSGameInstance>(LocalPlayer->GetGameInstance());
	}

	return GameInstance;
}


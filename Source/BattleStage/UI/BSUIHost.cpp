// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSUIHost.h"
#include "BSGameInstance.h"
#include "Slate/BSHostedWidget.h"


void UBSUIHost::Open()
{
	// Do Nothing
}

void UBSUIHost::Close()
{
	if (ReturnHost)
	{
		ReturnHost->Open();
	}
}

UBSGameInstance* UBSUIHost::GetGameInstance() const
{
	UBSGameInstance* GameInstance = nullptr;
	const ULocalPlayer* const LocalPlayer = PlayerContext.GetLocalPlayer();

	if (LocalPlayer)
	{
		GameInstance = Cast<UBSGameInstance>(LocalPlayer->GetGameInstance());
	}

	return GameInstance;
}

void UBSUIHost::SetLocalPlayerContext(APlayerController* PlayerController)
{
	PlayerContext = FLocalPlayerContext{ PlayerController };
}

const FLocalPlayerContext& UBSUIHost::GetLocalPlayerContext() const
{
	return PlayerContext;
}

void UBSUIHost::SetReturnHost(UBSUIHost* InReturnHost)
{
	ReturnHost = InReturnHost;
}
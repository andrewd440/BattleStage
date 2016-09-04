// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#ifndef __BATTLESTAGE_H__
#define __BATTLESTAGE_H__

#include "Engine.h"
#include "UMG.h"
#include "Net/UnrealNetwork.h"

#include "BSGameInstance.h"
#include "BSGameMode.h"
#include "BSGameState.h"
#include "BSPlayerState.h"
#include "BSPlayerController.h"
#include "BSCharacter.h"
#include "BSTypes.h"


DECLARE_LOG_CATEGORY_EXTERN(BattleStage, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(BattleStageOnline, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(BattleStageUI, Log, All);

#define WEAPON_CHANNEL ECC_GameTraceChannel1

#endif

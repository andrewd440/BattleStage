// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "BSMatchConfig.generated.h"

/**
 * Data asset that exposes data regarding available 
 * game type configurations. Maps, gamemodes, max/min players
 * for maps, etc.
 */
UCLASS(Abstract, Blueprintable)
class BATTLESTAGE_API UBSMatchConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	const TArray<FString>& GetAllMapNames() const { return MapNames; }
	
	const TArray<FString>& GetAllGameModeNames() const { return GameModeNames; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GameTypes)
	TArray<FString> MapNames;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GameTypes)
	TArray<FString> GameModeNames;
};

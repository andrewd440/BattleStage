// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BSTypes.generated.h"

//-----------------------------------------------------------------
// Information used when spawning a decal in-game.
//-----------------------------------------------------------------
USTRUCT()
struct FDecalInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	UMaterial* Material = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FVector DecalSize = FVector{20.0f, 20.0f, 20.0f};

	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 1.0f;
};
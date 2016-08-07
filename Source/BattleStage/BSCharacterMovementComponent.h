// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "BSCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESTAGE_API UBSCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UBSCharacterMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual float GetMaxSpeed() const override;
};

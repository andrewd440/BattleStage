// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSCharacterMovementComponent.h"
#include "BSCharacter.h"

float UBSCharacterMovementComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	ABSCharacter* BSCharacter = Cast<ABSCharacter>(CharacterOwner);
	if (BSCharacter && MovementMode == EMovementMode::MOVE_Walking)
	{
		MaxSpeed *= BSCharacter->GetMovementModifier();
	}

	return MaxSpeed;
}

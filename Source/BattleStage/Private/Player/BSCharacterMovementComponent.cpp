// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSCharacterMovementComponent.h"

UBSCharacterMovementComponent::UBSCharacterMovementComponent(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	bCanWalkOffLedgesWhenCrouching = true;
	MaxWalkSpeed = 450.f;

	NavAgentProps.bCanCrouch = true;
	NavAgentProps.bCanSwim = false;
}

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

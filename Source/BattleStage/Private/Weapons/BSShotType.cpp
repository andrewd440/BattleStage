// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSShotType.h"
#include "BSWeapon.h"

class UWorld* UBSShotType::GetWorld() const
{
	return GetWeapon()->GetWorld();
}

bool UBSShotType::IsSupportedForNetworking() const
{
	return true;
}

ABSWeapon* UBSShotType::GetWeapon() const
{
	return static_cast<ABSWeapon*>(GetOuter());
}

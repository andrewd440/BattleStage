// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSHUDWidget_Ammo.h"
#include "Weapons/BSWeapon.h"

void UBSHUDWidget_Ammo::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (BSCharacter.IsValid())
	{
		if (ABSWeapon* Weapon = BSCharacter->GetEquippedWeapon())
		{
			RemainingClip = Weapon->GetRemainingClip();
			RemainingAmmo = Weapon->GetRemainingAmmo();
		}
	}
}

void UBSHUDWidget_Ammo::NativeConstruct()
{
	Super::NativeConstruct();

	BSCharacter = Cast<ABSCharacter>(GetOwningPlayerPawn());
}

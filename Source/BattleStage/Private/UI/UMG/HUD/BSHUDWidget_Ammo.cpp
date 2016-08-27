// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSHUDWidget_Ammo.h"
#include "BSWeapon.h"

void UBSHUDWidget_Ammo::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ABSCharacter* Character = Cast<ABSCharacter>(GetOwningPlayerPawn()))
	{
		if (ABSWeapon* Weapon = Character->GetEquippedWeapon())
		{
			const int32 NewRemainingClip = Weapon->GetRemainingClip();			
			if (NewRemainingClip != RemainingClip)
			{
				RemainingClipText->SetText(FText::AsNumber(NewRemainingClip));
				RemainingClip = NewRemainingClip;
			}
			
			const int32 NewRemainingAmmo = Weapon->GetRemainingAmmo();
			if (NewRemainingAmmo != RemainingAmmo)
			{
				RemainingAmmoText->SetText(FText::AsNumber(NewRemainingAmmo));
				RemainingAmmo = NewRemainingAmmo;
			}

		}
	}
}

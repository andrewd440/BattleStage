// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSHUD.h"
#include "UserWidget.h"
#include "BSCharacter.h"
#include "Weapons/BSWeapon.h"
#include "CanvasItem.h"

ABSHUD::ABSHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	LastWeaponHitTime = -MAX_FLT;
}

void ABSHUD::NotifyWeaponHit()
{
	LastWeaponHitTime = GetWorld()->GetTimeSeconds();

	if (HitIndicatorSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), HitIndicatorSound);
	}

	OnNotifyWeaponHit.Broadcast();
}

void ABSHUD::NotifyReceivedDamage(const FVector& InDamageOrigin)
{
	LastDamagedTime = GetWorld()->GetTimeSeconds();

	DamageOrigin = InDamageOrigin;

	OnNotifyReceivedDamage.Broadcast(InDamageOrigin);
}

void ABSHUD::BeginPlay()
{
	Super::BeginPlay();

	if (HUDLayoutClass)
	{
		HUDLayoutWidget = CreateWidget<UUserWidget>(PlayerOwner, HUDLayoutClass);
		HUDLayoutWidget->AddToViewport();
	}
}

void ABSHUD::DrawHUD()
{
	Super::DrawHUD();

	if (ABSCharacter* Character = Cast<ABSCharacter>(GetOwningPawn()))
	{
		DrawCrosshair(*Character);
		DrawLowHealthOverlay(*Character);
	}

	DrawDamageIndicator();
}

void ABSHUD::DrawCrosshair(ABSCharacter& Character)
{
	if (Character.IsFirstPerson())
	{
		if (ABSWeapon* Weapon = Character.GetEquippedWeapon())
		{
			const float SPREAD_SCALE = 6.f;
			const float AimSpread = Weapon->GetCurrentSpread() * SPREAD_SCALE;

			Canvas->SetDrawColor(255, 255, 255, 190);

			float CenterX = 0.f, CenterY = 0.f;
			Canvas->GetCenter(CenterX, CenterY);

			// Center
			Canvas->DrawIcon(Crosshair[(uint8)ECrosshairPosition::Center], 
				CenterX - Crosshair[(uint8)ECrosshairPosition::Center].UL / 2.f,
				CenterY - Crosshair[(uint8)ECrosshairPosition::Center].VL / 2.f);

			// Top
			Canvas->DrawIcon(Crosshair[(uint8)ECrosshairPosition::Top],
				CenterX - Crosshair[(uint8)ECrosshairPosition::Top].UL / 2.f,
				CenterY - AimSpread - Crosshair[(uint8)ECrosshairPosition::Top].VL);

			// Bottom
			Canvas->DrawIcon(Crosshair[(uint8)ECrosshairPosition::Bottom],
				CenterX - Crosshair[(uint8)ECrosshairPosition::Bottom].UL / 2.f,
				CenterY + AimSpread);

			// Left
			Canvas->DrawIcon(Crosshair[(uint8)ECrosshairPosition::Left],
				CenterX - AimSpread - Crosshair[(uint8)ECrosshairPosition::Left].UL,
				CenterY - Crosshair[(uint8)ECrosshairPosition::Left].VL / 2.f);

			// Right
			Canvas->DrawIcon(Crosshair[(uint8)ECrosshairPosition::Right],
				CenterX + AimSpread,
				CenterY - Crosshair[(uint8)ECrosshairPosition::Right].VL / 2.f);

			// Draw hit marker if within time of last hit
			const float SinceLastHit = GetWorld()->GetTimeSeconds() - LastWeaponHitTime;
			if (SinceLastHit < HitIndicationDuration)
			{
				// Scale alpha based on time since last hit
				const float Alpha = 1.f - SinceLastHit / HitIndicationDuration;
				Canvas->SetDrawColor(255, 52, 52, Alpha * 255);

				Canvas->DrawIcon(HitIndicator,
					CenterX - HitIndicator.UL / 2.f,
					CenterY - HitIndicator.VL / 2.f);
			}
		}
	}
}

void ABSHUD::DrawLowHealthOverlay(ABSCharacter& Character)
{
	const float MaxHealth = ABSCharacter::StaticClass()->GetDefaultObject<ABSCharacter>()->GetHealth();
	const float CurrentHealth = Character.GetHealth();

	if (CurrentHealth < MaxHealth)
	{
		const float Alpha = 1.f - CurrentHealth / MaxHealth;
		const FLinearColor OverlayColor{ 1, 1, 1, Alpha };

		DrawTexture(LowHealthOverlay, 0, 0, Canvas->SizeX, Canvas->SizeY, 0, 0, 1.f, 1.f, OverlayColor);
	}
}

void ABSHUD::DrawDamageIndicator()
{
	// Draw damage indicator if within time of last damage event
	if (APawn* Pawn = GetOwningPawn())
	{
		const float SinceLastDamage = GetWorld()->GetTimeSeconds() - LastDamagedTime;
		if (SinceLastDamage < DamageIndicatorDuration)
		{
			// Get rotation needed for texture to orient towards damage origin
			const FVector CurrentPosition = Pawn->GetActorLocation();
			const FVector CurrentForward = Pawn->GetActorForwardVector().GetSafeNormal2D();
			const FVector DamageDirection = (DamageOrigin - CurrentPosition).GetSafeNormal2D();

			float DamageRotation = FMath::Acos(FVector::DotProduct(CurrentForward, DamageDirection));

			// Determine if direction is to the left or right, negate rotation if on left
			if (FVector::CrossProduct(CurrentForward, DamageDirection).Z < 0)
				DamageRotation = -DamageRotation;

			// Draw indicator
			const float Alpha = 1.f - (SinceLastDamage / DamageIndicatorDuration);
			const FLinearColor TextureColor{ 1, 1, 1, Alpha };

			float CenterX = 0.f, CenterY = 0.f;
			Canvas->GetCenter(CenterX, CenterY);

			// Draw full texture 1:1 texel ratio and rotate by center
			DrawTexture(DamageIndicator,
				CenterX - DamageIndicator->GetSizeX() / 2.f,
				CenterY - DamageIndicator->GetSizeY() / 2.f,
				DamageIndicator->GetSizeX(),
				DamageIndicator->GetSizeY(),
				0.f, 0.f, 1.f, 1.f,
				TextureColor,
				BLEND_Translucent,
				1.f, false,
				FMath::RadiansToDegrees(DamageRotation),
				FVector2D{ 0.5f, 0.5f });
		}
	}
}

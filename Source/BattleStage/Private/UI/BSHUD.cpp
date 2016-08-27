// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "CanvasItem.h"
#include "BSHUD.h"
#include "BSCharacter.h"
#include "BSWeapon.h"

#include "BSScoreboardWidget.h"
#include "BSHUDLayout.h"
#include "BSGameState.h"
#include "BSPlayerState.h"

#define LOCTEXT_NAMESPACE "BattleStage.HUD"

ABSHUD::ABSHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	LastWeaponHitTime = LastDamagedTime = -MAX_FLT;
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

bool ABSHUD::IsGameScoreboardUp() const
{
	return GameScoreboard && GameScoreboard->IsInViewport();
}

void ABSHUD::ShowGameScoreboard(const bool bShowScoreboard)
{
	if (bShowScoreboard && !IsGameScoreboardUp())
	{
		if (GameScoreboard)
		{
			GameScoreboard->AddToViewport();
		}
	}
	else if (!bShowScoreboard && IsGameScoreboardUp())
	{
		GameScoreboard->RemoveFromParent();
	}
}

void ABSHUD::BeginPlay()
{
	Super::BeginPlay();

	if (HUDLayoutClass)
	{
		HUDLayout = CreateWidget<UBSHUDLayout>(PlayerOwner, HUDLayoutClass);
		HUDLayout->AddToViewport();
	}

	if (ABSGameState* const GameState = GetWorld()->GetGameState<ABSGameState>())
	{
		if (const ABSGameMode* GameMode = Cast<const ABSGameMode>(GameState->GameModeClass->GetDefaultObject()))
		{
			GameScoreboard = CreateWidget<UBSScoreboardWidget>(PlayerOwner, GameMode->GetScoreboardWidget());
		}

		GameState->OnScoreEvent().AddUObject(this, &ABSHUD::OnScoreEventReceived);
	}
}

void ABSHUD::OnScoreEventReceived(const FScoreEvent& ScoreEvent)
{
	constexpr int32 MaxEventCount = 3;
	constexpr float EventFeedItemLifetime = 4.f;

	if (EventFeed.Num() >= MaxEventCount)
	{
		EventFeed.RemoveAt(1, 1, false);
	}

	// Construct event feed item
	const float GameTime = GetWorld()->GetTimeSeconds();
	FEventFeedItem FeedItem;
	FeedItem.ExpireTime = GetWorld()->GetTimeSeconds() + EventFeedItemLifetime;
	FeedItem.ScorerName = FText::FromString(ScoreEvent.Scorer->PlayerName);
	FeedItem.bPlayerScore = PlayerOwner ? ScoreEvent.Scorer == PlayerOwner->PlayerState : false;

	if (ScoreEvent.Victim.IsValid())
	{
		FeedItem.VictemName = FText::FromString(ScoreEvent.Victim->PlayerName);
		FeedItem.bPlayerVictim = PlayerOwner ? ScoreEvent.Victim == PlayerOwner->PlayerState : false;
	}
	else
	{
		FeedItem.bPlayerVictim = false;
	}

	FeedItem.Type = ScoreEvent.Type;	
	
	// Construct personal message if needed
	{
		const FString KilledText = TEXT("Killed ");
		const FString DeathText = TEXT("Died");
		const FString SuicideText = TEXT("Self Execution");
		const FString KilledByText = TEXT("Killed by ");

		FString MessageString;
		if (FeedItem.bPlayerScore)
		{
			if (FeedItem.Type == EScoreType::Kill)
			{
				MessageString = KilledText + ScoreEvent.Victim->PlayerName;
				MessageString += FString::Printf(TEXT("  +%d"), ScoreEvent.ScorerPoints);
			}
			else if (FeedItem.Type == EScoreType::Death)
			{
				MessageString = DeathText;
			}
			else
			{
				MessageString = SuicideText;
			}
		}
		else if (FeedItem.bPlayerVictim)
		{
			MessageString = KilledByText + ScoreEvent.Scorer->PlayerName;
		}

		LastPersonalMessage.Message = FText::FromString(MessageString);
		LastPersonalMessage.ExpireTime = GameTime + EventFeedItemLifetime;
	}

	EventFeed.Add(std::move(FeedItem));
}

void ABSHUD::DrawHUD()
{
	Super::DrawHUD();

	UIScale = Canvas->ClipY / 1080.f;

	if (ABSCharacter* Character = Cast<ABSCharacter>(GetOwningPawn()))
	{
		DrawCrosshair(*Character);
		DrawLowHealthOverlay(*Character);
	}

	DrawDamageIndicator();
	DrawEventFeed();
	DrawPersonalEventMessage();
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
				CenterX - Crosshair[(uint8)ECrosshairPosition::Center].UL * UIScale / 2.f,
				CenterY - Crosshair[(uint8)ECrosshairPosition::Center].VL * UIScale / 2.f,
				UIScale);

			// Top
			Canvas->DrawIcon(Crosshair[(uint8)ECrosshairPosition::Top],
				CenterX - Crosshair[(uint8)ECrosshairPosition::Top].UL * UIScale / 2.f,
				CenterY - ((AimSpread + Crosshair[(uint8)ECrosshairPosition::Top].VL) * UIScale),
				UIScale);

			// Bottom
			Canvas->DrawIcon(Crosshair[(uint8)ECrosshairPosition::Bottom],
				CenterX - Crosshair[(uint8)ECrosshairPosition::Bottom].UL * UIScale / 2.f,
				CenterY + AimSpread * UIScale,
				UIScale);

			// Left
			Canvas->DrawIcon(Crosshair[(uint8)ECrosshairPosition::Left],
				CenterX - ((AimSpread + Crosshair[(uint8)ECrosshairPosition::Left].UL) * UIScale),
				CenterY - Crosshair[(uint8)ECrosshairPosition::Left].VL * UIScale / 2.f,
				UIScale);

			// Right
			Canvas->DrawIcon(Crosshair[(uint8)ECrosshairPosition::Right],
				CenterX + AimSpread * UIScale,
				CenterY - Crosshair[(uint8)ECrosshairPosition::Right].VL * UIScale / 2.f,
				UIScale);

			// Draw hit marker if within time of last hit
			const float SinceLastHit = GetWorld()->GetTimeSeconds() - LastWeaponHitTime;
			if (SinceLastHit < HitIndicationDuration)
			{
				// Scale alpha based on time since last hit
				const float Alpha = 1.f - SinceLastHit / HitIndicationDuration;
				Canvas->SetDrawColor(255, 52, 52, Alpha * 255);

				Canvas->DrawIcon(HitIndicator,
					CenterX - HitIndicator.UL * UIScale / 2.f,
					CenterY - HitIndicator.VL * UIScale / 2.f,
					UIScale);
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
				CenterX - DamageIndicator->GetSizeX() * UIScale / 2.f,
				CenterY - DamageIndicator->GetSizeY() * UIScale / 2.f,
				DamageIndicator->GetSizeX(),
				DamageIndicator->GetSizeY(),
				0.f, 0.f, 1.f, 1.f,
				TextureColor,
				BLEND_Translucent,
				UIScale, false,
				FMath::RadiansToDegrees(DamageRotation),
				FVector2D{ 0.5f, 0.5f });
		}
	}
}

void ABSHUD::DrawEventFeed()
{
	const float GameTime = GetWorld()->GetTimeSeconds();
	EventFeed.RemoveAll([GameTime](const FEventFeedItem& Item) { return Item.ExpireTime <= GameTime; });

	if (EventFeed.Num() > 0)
	{
		Canvas->SetDrawColor(FColor::White);

		FLinearColor TeamColor{ 0.f, 0.29f, 0.57f, 1.f };
		FLinearColor EnemyColor{ 1.f, 0.07f, .0f, 1.f };
		FLinearColor DefaultColor = FLinearColor::White;

		const FText KilledText = LOCTEXT("Killed", "killed");
		const FText DeathText = LOCTEXT("Died", "died");
		const FText SuicideText = LOCTEXT("Suicide", "opted out");

		const FVector2D TextPadding{ 5.f * UIScale, 10.f * UIScale };
		float OffsetY = 100.f * UIScale;

		FCanvasTextItem TextItem(FVector2D::ZeroVector, FText::GetEmpty(), NormalFont, FLinearColor::White);
		TextItem.Scale = FVector2D{ UIScale, UIScale };
		TextItem.EnableShadow(FLinearColor::Black);

		constexpr float TextAlphaReduction = 0.3f;

		for (int32 i = EventFeed.Num() - 1; i >= 0; --i)
		{
			const FEventFeedItem& Event = EventFeed[i];

			float OffsetX = 40.f * UIScale;
			TextItem.Text = Event.ScorerName;
			TextItem.SetColor((Event.bPlayerScore) ? TeamColor : EnemyColor);
			Canvas->DrawItem(TextItem, OffsetX, OffsetY);
			OffsetX += TextItem.DrawnSize.X + TextPadding.X;

			TextItem.SetColor(DefaultColor);

			if (Event.Type == EScoreType::Kill)
			{
				TextItem.Text = KilledText;
				Canvas->DrawItem(TextItem, OffsetX, OffsetY);
				OffsetX += TextItem.DrawnSize.X + TextPadding.X;

				TextItem.Text = Event.VictemName;
				TextItem.SetColor((Event.bPlayerVictim) ? TeamColor : EnemyColor);
				Canvas->DrawItem(TextItem, OffsetX, OffsetY);
				OffsetX += TextItem.DrawnSize.X;
			}
			else if (Event.Type == EScoreType::Death)
			{
				TextItem.Text = DeathText;
				Canvas->DrawItem(TextItem, OffsetX, OffsetY);
			}
			else
			{
				TextItem.Text = SuicideText;
				Canvas->DrawItem(TextItem, OffsetX, OffsetY);
			}

			OffsetY += TextPadding.Y + TextItem.DrawnSize.Y;			

			// Decrease text alpha from most recent event
			DefaultColor.A -= TextAlphaReduction;
			EnemyColor.A -= TextAlphaReduction;
			TeamColor.A -= TextAlphaReduction;
		}
	}
}

void ABSHUD::DrawPersonalEventMessage()
{
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (GameTime <= LastPersonalMessage.ExpireTime)
	{
		Canvas->SetDrawColor(FColor::White);

		FCanvasTextItem TextItem{ FVector2D::ZeroVector, LastPersonalMessage.Message, LargeFont, FLinearColor::White };
		TextItem.Scale = FVector2D{ UIScale, UIScale };
		TextItem.EnableShadow(FLinearColor::Black);

		float CenterX, CenterY;
		Canvas->GetCenter(CenterX, CenterY);

		FVector2D TextSize;
		Canvas->StrLen(LargeFont, LastPersonalMessage.Message.ToString(), TextSize.X, TextSize.Y);

		const FVector2D BaseDrawPosition{ CenterX, Canvas->ClipY - (300.f * UIScale) };
		const FVector2D DrawPositionOffset{ TextSize.X / 2.f, TextSize.Y / 2.f };

		TextItem.Position = BaseDrawPosition - DrawPositionOffset * TextItem.Scale;

		Canvas->DrawItem(TextItem);		
	}
}

#undef LOCTEXT_NAMESPACE
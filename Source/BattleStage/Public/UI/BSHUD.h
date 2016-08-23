// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "BSHUD.generated.h"

class UUserWidget;
class ABSCharacter;

struct FScoreEvent;
enum class EScoreType : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNotifyWeaponHitEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNotifyReceivedDamageEvent, FVector, IncomingDirection);

UENUM()
enum class ECrosshairPosition : uint8
{
	Center,
	Top,
	Bottom,
	Left,
	Right,
	Max
};

/**
 * 
 */
UCLASS()
class BATTLESTAGE_API ABSHUD : public AHUD
{
	GENERATED_UCLASS_BODY()

public:
	//-----------------------------------------------------------------
	// HUD Events
	//-----------------------------------------------------------------

	/** Broadcasted when a weapon hit has occurred. */
	UPROPERTY(BlueprintAssignable, Category = HUD)
	FOnNotifyWeaponHitEvent OnNotifyWeaponHit;

	/** Broadcasted when a received damage occurs. */
	UPROPERTY(BlueprintAssignable, Category = HUD)
	FOnNotifyReceivedDamageEvent OnNotifyReceivedDamage;

public:
	/** Notifies the HUD when a controlled weapon has hit a character. */
	void NotifyWeaponHit();

	/** 
	 * Notifies the HUD that the controlled character has received damage in game. 
	 * Will activate HUD based effects that respond to damage events.
	 * 
	 * @param SourcePosition	The world position of the source of the damage.
	 */
	void NotifyReceivedDamage(const FVector& SourcePosition);

	/** Check if the game scoreboard is up */
	bool IsGameScoreboardUp() const;

	/** Show/Hide the game scoreboard */
	void ShowGameScoreboard(const bool bShowScoreboard);

	/** AHUD Interface Begin */
	virtual void DrawHUD() override;
	/** AHUD Interface End */

	/** AActor Interface Begin */
	virtual void BeginPlay() override;
	/** AActor Interface End */

protected:
	virtual void OnScoreEventReceived(const FScoreEvent& ScoreEvent);

private:
	/** 
	 * Draws the weapon spread based crosshair. The weapon hit marker
	 * will be drawn if last hit was within hit marker duration.
	 * 
	 * @param Character	The owned character.
	 */
	void DrawCrosshair(ABSCharacter& Character);
	
	/**
	* Draws the low health full screen overlay based on the current
	* health of the owned character.
	* 
	* @param Character	The owned character.
	*/
	void DrawLowHealthOverlay(ABSCharacter& Character);

	/**
	* Draws a directional damage indicator oriented towards the last
	* location damage was received from. Only displayed if the last damage
	* event was within DamageIndicatorDuration.
	*/
	void DrawDamageIndicator();

	/**
	 * Draws the game event feed.
	 */
	void DrawEventFeed();

protected:
	/** Widget type for the HUD layout, container for in-game HUD */
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	TSubclassOf<class UBSHUDLayout> HUDLayoutClass;

	/** Weapon crosshair components */
	UPROPERTY(EditAnywhere, Category = HUD)
	FCanvasIcon Crosshair[ECrosshairPosition::Max];

	/** Indicator displayed on weapon hit */
	UPROPERTY(EditAnywhere, Category = HitIndication)
	FCanvasIcon HitIndicator;

	/** Seconds to display hit indicator */
	UPROPERTY(EditDefaultsOnly, Category = HitIndication)
	float HitIndicationDuration = 1.f;

	/** Sound to play on weapon hit */
	UPROPERTY(EditDefaultsOnly, Category = HitIndication)
	USoundBase* HitIndicatorSound = nullptr;

	/** Indicator used when damaged to indicate direction of incoming damage */
	UPROPERTY(EditAnywhere, Category = DamageIndication)
	UTexture2D* DamageIndicator = nullptr;

	/** Seconds to display damage indicator */
	UPROPERTY(EditDefaultsOnly, Category = DamageIndication)
	float DamageIndicatorDuration = 1.f;

	/** Full screen texture applied with modulated alpha relative to current health */
	UPROPERTY(EditAnywhere, Category = DamageIndication)
	UTexture2D* LowHealthOverlay = nullptr;

	UPROPERTY(EditAnywhere, Category = EventFeed)
	UFont* EventFeedFont = nullptr;

private:
	UPROPERTY()
	UBSHUDLayout* HUDLayout = nullptr;

	UPROPERTY()
	UBSScoreboardWidget* GameScoreboard = nullptr;

	/** Game time of last weapon hit */
	float LastWeaponHitTime = 0.f;
	
	/** Game time of last time damage was received */
	float LastDamagedTime = 0.f;

	/** Origin position of last damage event */
	FVector DamageOrigin = FVector::ZeroVector;

	/** Score event details used in the event feed */
	struct FEventFeedItem
	{
		FText ScorerName;
		FText VictemName;
		EScoreType Type;
		float ExpireTime;
		uint32 bPlayerScore : 1; // Are we the scorer
		uint32 bPlayerVictim : 1; // Are we the victim
	};

	/** 
	 * Text representation of events in the event feed and the number
	 * of seconds each event has been in the feed. 
	 */
	TArray<FEventFeedItem> EventFeed;

	/** Scaling for the UI during a draw call. Based on scale factor relative to 1080p. */
	float UIScale = 1.f;
};

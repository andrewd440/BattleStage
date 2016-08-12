// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "BSHUD.generated.h"

class UUserWidget;
class ABSCharacter;

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
	/** Notifies the HUD when a controlled weapon has hit a character. */
	void NotifyWeaponHit();

	/** 
	 * Notifies the HUD that the controlled character has received damage in game. 
	 * Will activate HUD based effects that respond to damage events.
	 * 
	 * @param SourcePosition	The world position of the source of the damage.
	 */
	void NotifyReceivedDamage(const FVector& SourcePosition);

	//-----------------------------------------------------------------
	// HUD Events
	//-----------------------------------------------------------------

	/** Broadcasted when a weapon hit has occurred. */
	UPROPERTY(BlueprintAssignable, Category = HUD)
	FOnNotifyWeaponHitEvent OnNotifyWeaponHit;

	/** Broadcasted when a received damage occurs. */
	UPROPERTY(BlueprintAssignable, Category = HUD)
	FOnNotifyReceivedDamageEvent OnNotifyReceivedDamage;

	/** AHUD Interface Begin */
	virtual void DrawHUD() override;
	/** AHUD Interface End */

	/** AActor Interface Begin */
	virtual void BeginPlay() override;
	/** AActor Interface End */

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

protected:
	/** Widget type for the HUD layout, container for in-game HUD */
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	TSubclassOf<UUserWidget> HUDLayoutClass;

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
	USoundBase* HitIndicatorSound;

	/** Indicator used when damaged to indicate direction of incoming damage */
	UPROPERTY(EditAnywhere, Category = DamageIndication)
	UTexture2D* DamageIndicator;

	/** Seconds to display damage indicator */
	UPROPERTY(EditDefaultsOnly, Category = DamageIndication)
	float DamageIndicatorDuration = 1.f;

	/** Full screen texture applied with modulated alpha relative to current health */
	UPROPERTY(EditAnywhere, Category = DamageIndication)
	UTexture2D* LowHealthOverlay;

private:
	UUserWidget* HUDLayoutWidget = nullptr;

	/** Game time of last weapon hit */
	float LastWeaponHitTime = 0.f;
	
	/** Game time of last time damage was received */
	float LastDamagedTime = 0.f;

	/** Origin position of last damage event */
	FVector DamageOrigin = FVector::ZeroVector;
};

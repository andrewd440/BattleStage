// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "BSPlayerController.generated.h"

class ABSCharacter;

/**
 * 
 */
UCLASS()
class BATTLESTAGE_API ABSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABSPlayerController();

	UFUNCTION(Client, Unreliable)
	virtual void ClientHearSound(USoundBase* Sound, AActor* SourceActor, const FVector_NetQuantize SoundLocation) const;

	/** Respawn */
	virtual void UnFreeze() override;

	/** Set spectator cam looking at pawn */
	virtual void PawnPendingDestroy(APawn* inPawn) override;

	UFUNCTION(Reliable, Client)
	void ClientSetSpectatorCamera(const FVector CameraLocation, const FRotator CameraRotation);

	/**
	* Notifies the controller it has hit a character with a weapon under the control of the it's pawn.
	* Results in a RPC on the client that notifies a weapon hit.
	*/
	virtual void NotifyWeaponHit();

	/**
	* Notifies the controller that the controlled character has received damage in game.
	* Results in a RPC on the client that notifies damage receieved.
	*
	* @param SourcePosition	The world position of the source of the damage.
	*/
	virtual void NotifyReceivedDamage(const FVector& SourcePosition);

	UFUNCTION(BlueprintCallable, Category = Menu)
	virtual void ToggleInGameMenu();
	
	/**
	* Shows or hides the in-game menu.
	* 
	* @param bShowMenu	True if the menu should be shown. False if it should be hidden.
	*/
	UFUNCTION(BlueprintCallable, Category = Menu)
	virtual void ShowInGameMenu(const bool bShowMenu);

	/**
	* Used to handle player requests to leave the current game and return to the main
	* menu. Delgates work to the GameState to handle client/host quit game behavior.
	*/
	UFUNCTION(BlueprintCallable, Category = PlayerController)
	void HandleReturnToMainMenu();

	virtual void ClientReturnToMainMenu_Implementation(const FString& ReturnReason) override;

protected:
	UFUNCTION(Client, Unreliable)
	void ClientNotifyWeaponHit();

	UFUNCTION(Client, Unreliable)
	void ClientNotifyReceivedDamage(const FVector& SourcePosition);

protected:
	/** Handles moving forward/backward */
	void OnMoveForward(float Val);

	/** Handles stafing movement, left and right */
	void OnMoveRight(float Val);

	/** Handles jump input */
	void OnJump();

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void OnTurnAtRate(float Rate);

	/**
	* Called via input to look up at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired look rate
	*/
	void OnLookUpRate(float Rate);

	void OnStartFire();
	void OnStopFire();
	void OnReload();
	void OnStartSprint();
	void OnToggleSprint();
	void OnStopSprint();
	void OnCrouch();

protected:
	/* The base turn rate of the player **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controller)
	float BaseTurnRate;

	/* The base look rate of the player **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controller)
	float BaseLookRate;

	/** Camera shake used when damage taken */
	UPROPERTY(EditDefaultsOnly, Category = Controller)
	TSubclassOf<UCameraShake> HitCameraShake;

	UPROPERTY(EditDefaultsOnly, Category = Menu)
	TSubclassOf<class UBSUserWidget> InGameMenuClass;

private:
	ABSCharacter* BSCharacter; //< If valid, the owning BSCharacter
	
	UBSUserWidget* InGameMenuWidget = nullptr;

	//-----------------------------------------------------------------
	// Begin AController Interface
	//-----------------------------------------------------------------
public:
	virtual void SetPawn(APawn* InPawn) override;
protected:
	virtual void SetupInputComponent() override;
	//-----------------------------------------------------------------
	// End AController Interface
	//-----------------------------------------------------------------
	
public:
	UFUNCTION(BlueprintCallable, Category = PlayerController)
	FORCEINLINE ABSCharacter* GetBSCharacter() const { return BSCharacter; }
};

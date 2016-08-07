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

private:
	ABSCharacter* BSCharacter; //< If valid, the owning BSCharacter
	
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

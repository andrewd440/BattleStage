// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSPlayerController.h"
#include "BSCharacter.h"
#include "Weapons/BSWeapon.h"
#include "Camera/PlayerCameraManager.h"

ABSPlayerController::ABSPlayerController()
	: Super()
	, BSCharacter(nullptr)
	, BaseTurnRate(45.0f)
	, BaseLookRate(45.0f)
{

}

void ABSPlayerController::UnFreeze()
{
	ServerRestartPlayer();
}

void ABSPlayerController::PawnPendingDestroy(APawn* inPawn)
{
	APawn* const Pawn = GetPawn();
	FVector DeathLocation = Pawn->GetActorLocation() + FVector{ 0, 0, 300 };
	FRotator CameraRotation{ -90.f, 0, 0 };

	// #bstodo Get better camera view and location

	Super::PawnPendingDestroy(inPawn);

	ClientSetSpectatorCamera(DeathLocation, CameraRotation);
}

void ABSPlayerController::ClientSetSpectatorCamera_Implementation(const FVector CameraLocation, const FRotator CameraRotation)
{
	SetInitialLocationAndRotation(CameraLocation, CameraRotation);
	SetViewTarget(this);
}

void ABSPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	BSCharacter = Cast<ABSCharacter>(InPawn);
}

void ABSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);
	
	InputComponent->BindAxis("MoveForward", this, &ABSPlayerController::OnMoveForward);
	InputComponent->BindAxis("MoveRight", this, &ABSPlayerController::OnMoveRight);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ABSPlayerController::OnJump);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &ABSPlayerController::AddYawInput);
	InputComponent->BindAxis("TurnRate", this, &ABSPlayerController::OnTurnAtRate);

	InputComponent->BindAxis("LookUp", this, &ABSPlayerController::AddPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &ABSPlayerController::OnLookUpRate);

	InputComponent->BindAction("Fire", IE_Pressed, this, &ABSPlayerController::OnStartFire);
	InputComponent->BindAction("Fire", IE_Released, this, &ABSPlayerController::OnStopFire);
	InputComponent->BindAction("Reload", IE_Pressed, this, &ABSPlayerController::OnReload);
}

void ABSPlayerController::ClientHearSound_Implementation(USoundBase* Sound, AActor* SourceActor, FVector_NetQuantize SoundLocation) const
{
	if (!Sound)
	{
		UE_LOG(BattleStage, Warning, TEXT("ABSPlayerController::ClientHearSound() is being sent a null sound."))
	}
	else
	{
		if (SourceActor && SoundLocation.IsZero())
		{
			UGameplayStatics::SpawnSoundAttached(Sound, SourceActor->GetRootComponent());
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, SoundLocation);
		}
	}
}

void ABSPlayerController::OnMoveForward(float Value)
{
	APawn* Pawn = GetPawn();
	if (Pawn && Value != 0.0f)
	{
		Pawn->AddMovementInput(GetActorForwardVector().GetSafeNormal2D() , Value);
	}
}

void ABSPlayerController::OnMoveRight(float Value)
{
	APawn* Pawn = GetPawn();
	if (Pawn && Value != 0.0f)
	{
		Pawn->AddMovementInput(GetActorRightVector().GetSafeNormal2D(), Value);
	}
}

void ABSPlayerController::OnJump()
{
	ACharacter* Character = GetCharacter();
	if (Character)
	{
		Character->Jump();
	}
}

void ABSPlayerController::OnTurnAtRate(float Rate)
{
	AddYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ABSPlayerController::OnLookUpRate(float Rate)
{
	AddPitchInput(Rate * BaseLookRate * GetWorld()->GetDeltaSeconds());
}

void ABSPlayerController::OnStartFire()
{
	if (BSCharacter)
	{
		BSCharacter->StartFire();
	}
}

void ABSPlayerController::OnStopFire()
{
	if (BSCharacter)
	{
		BSCharacter->StopFire();
	}
}

void ABSPlayerController::OnReload()
{
	if (BSCharacter)
	{
		BSCharacter->ReloadWeapon();
	}
}

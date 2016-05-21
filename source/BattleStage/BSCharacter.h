// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "BSCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class ABSCharacter : public ACharacter
{
	GENERATED_BODY()

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCamera;

	/** First person mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* FirstPersonMesh;

	/** Character mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* CharacterMesh;

protected:
	/** All weapon classes that are available to the character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TArray<TSubclassOf<class ABSWeapon>> AvailableWeapons;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = Weapon)
	class ABSWeapon* EquippedWeapon;

public:
	ABSCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** APawn interface */
	virtual void Tick(float DeltaSeconds) override;
	/** APawn interface end */

	UFUNCTION(BlueprintCallable, Category = Weapon)
	void EquipWeapon();

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEquipWeapon();
	void ServerEquipWeapon_Implementation();
	bool ServerEquipWeapon_Validate();

public:
	/** Returns FirstPersonCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }

	/** Returns FirstPersonMesh subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns CharacterMesh subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetCharacterMesh() const { return CharacterMesh; }
};


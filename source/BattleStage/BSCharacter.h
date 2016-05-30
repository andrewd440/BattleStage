// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "BSCharacter.generated.h"

class UInputComponent;
class ABSWeapon;

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

public:
	ABSCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void StartFire();
	virtual void StopFire();

	/** ACharacter interface */\
	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostInitializeComponents() override;
	/** ACharacter interface end */

	/** AActor interface */
	virtual void BeginPlay() override;
	/** AActor interface end*/

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TSubclassOf<ABSWeapon> DefaultWeaponClass = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = Weapon)
	ABSWeapon* Weapon = nullptr;

	// Socket name to attach equipped weapons
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	FName WeaponEquippedSocket;

private:
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void EquipWeapon();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEquipWeapon();

public:
	/** Returns FirstPersonCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }

	/** Returns FirstPersonMesh subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns CharacterMesh subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetCharacterMesh() const { return CharacterMesh; }

	/** Gets the name of the socket to attach equipped weapons. */
	FORCEINLINE FName GetWeaponEquippedSocket() const { return WeaponEquippedSocket; }
};


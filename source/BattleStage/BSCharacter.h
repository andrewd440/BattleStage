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

public:
	ABSCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void StartFire();
	virtual void StopFire();

	/**
	* Returns the dominate mesh used for this character. Will be the first person mesh
	* for first person characters, otherwise it will be the third person mesh.
	*/
	UFUNCTION(BlueprintCallable, Category = Mesh)
	USkeletalMeshComponent* GetActiveMesh() const;
	
	/**
	* Checks if this is a first person character.
	*/
	UFUNCTION(BlueprintCallable, Category = Mesh)
	bool IsFirstPerson() const;

	/** ACharacter interface */\
	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostInitializeComponents() override;
	/** ACharacter interface end */

	/** AActor interface */
	virtual void BeginPlay() override;

	/** 
	 * Play Animation Montage on the character mesh. Will play on the first person mesh if
	 * this is a first person character, otherwise it will be played on the third person mesh.
	 */
	virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) override;

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

	/** Returns ThirdPersonMesh subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetThirdPersonMesh() const { return GetMesh(); }

	/** Gets the name of the socket to attach equipped weapons. */
	FORCEINLINE FName GetWeaponEquippedSocket() const { return WeaponEquippedSocket; }

	/** Gets the currently equipped weapon. Null if no weapon is equipped. */
	UFUNCTION(BlueprintCallable, Category = Mesh)
	FORCEINLINE ABSWeapon* GetEquippedWeapon() const { return Weapon; }
};


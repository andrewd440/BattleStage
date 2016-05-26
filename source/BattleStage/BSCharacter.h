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

	virtual void Fire() const;

	/** APawn interface */
	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaSeconds) override;
	/** APawn interface end */

	/** AActor interface */
	virtual void BeginPlay() override;
	/** AActor interface end */

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TSubclassOf<ABSWeapon> WeaponClass;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = Weapon)
	ABSWeapon* Weapon = nullptr;

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
};


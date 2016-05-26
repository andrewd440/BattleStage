// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BSProjectile.h"
#include "BSWeapon.generated.h"

class ABSCharacter;
class USoundBase;
class UAnimMontage;

UCLASS(Blueprintable, Abstract, NotPlaceable, Config = Game)
class BATTLESTAGE_API ABSWeapon : public AActor
{
	GENERATED_BODY()

public:
	static FName FireEffectName; // Name of the FireEffect particle system component

public:
	// Sets default values for this actor's properties
	ABSWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual void OnEquip(ABSCharacter* Character);

	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual void OnUnequip();

	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual void BeginFireSequence();

	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual bool CanFire() const { return true; }

	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual void Fire();

	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual void EndFireSequence();

protected:
	/**
	* Gets the rotation of a projectile to be fired from this weapon.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = Weapon)
	FRotator GetFireRotation() const;
	virtual FRotator GetFireRotation_Implementation() const;

	/**
	* Gets the world location of a projectile to be fired from this weapon.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = Weapon)
	FVector GetFireLocation() const;
	virtual FVector GetFireLocation_Implementation() const;

protected:
	// The character that has this weapon equipped. 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = Weapon)
	class ABSCharacter* CharacterOwner; //#bstodo May not need. Could use Owner.

	// Mesh for the weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh;

	// The projectile class fired by this weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TSubclassOf<class ABSProjectile> ProjectileClass;

	// Particle effect when the weapon is fired
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	class UParticleSystemComponent* MuzzleEffect;

	// Offset to fire the projectile from
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	FVector FireOffset;

	// Target relative offset of the weapon from it's attached socket
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	FVector SocketOffset;

	//-----------------------------------------------------------------
	// Weapon Sounds
	//-----------------------------------------------------------------

	// Sound effect on weapon begin fire sequence
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	USoundBase* BeginFireSequenceSound = nullptr;

	// Sound effect when the weapon is fire
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	USoundBase* FireSound = nullptr;

	// Sound effect on weapon end fire sequence
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	USoundBase* EndFireSequenceSound = nullptr;
	
	//-----------------------------------------------------------------
	// Weapon Animation 
	//-----------------------------------------------------------------
	
	// Played when the weapon is equipped
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	UAnimMontage* EquipAnim = nullptr;

	// Played when the weapon is unequipped
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	UAnimMontage* UnequipAnim = nullptr;

	// Played when the weapon is fired
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	UAnimMontage* FireAnim = nullptr;

protected:	
	/**
	* Spawns the weapon projectile. ProjectileClass must be non-null.
	*/
	virtual void SpawnProjectile();

	/**
	* Plays the fire effects for the weapon. Only plays effect for the
	* weapon, not projectile.
	*/
	virtual void PlayFireEffects();

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerBeginFireSequence();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEndFireSequence();

private:
	// Toggle flag that indicates that the server fired a shot when changed.
	UPROPERTY(ReplicatedUsing=OnRep_ServerFired)
	uint32 bServerFired:1;

private:
	UFUNCTION()
	void OnRep_ServerFired();
};

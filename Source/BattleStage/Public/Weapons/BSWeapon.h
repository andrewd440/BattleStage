// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"

#include "BSCharacter.h"
#include "BSShotType.h"

#include "BSWeapon.generated.h"

class ABSCharacter;
class USoundBase;
class UAnimMontage;

UENUM()
enum class EWeaponState
{
	Inactive,		// Is not equipped
	Active,			// Is equipped
	Equipping,		// In equipping sequence
	Unequipping,	// In unequipping sequence
	Firing,			// Trigger is held
	Reloading		// In reloading sequence
};

USTRUCT()
struct FWeaponStats
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ammo)
	int32 MaxAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ammo)
	int32 ClipSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BaseDamage;

	// Seconds betweens rounds
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FireRate;

	// Reload time in seconds
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ReloadSpeed;

	// Determines how fast weapon spread and recoil offsets are reset after firing the weapon.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Accuracy)
	float Stability;

	// Angle, in degrees, of spread for the weapon. This is the max half cone angle that shots fired from
	// this weapon will be directed.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin = "0.0", ClampMax = "45.0", UIMin = "0.0", UIMax = "45.0"), Category = Accuracy)
	float BaseSpread;

	// Factor added to with BaseSpread when standing
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "45.0", UIMin = "0.0", UIMax = "45.0"), Category = Accuracy)
	float StandingSpreadIncrement;

	// Factor added to BaseSpread when moving
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "45.0", UIMin = "0.0", UIMax = "45.0"), Category = Accuracy)
	float MovingSpreadIncrement;

	// Factor added to BaseSpread per shot
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "45.0", UIMin = "0.0", UIMax = "45.0"), Category = Recoil)
	float RecoilSpreadIncrement;

	// The average direction that recoil from each shot will push the weapon.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Recoil)
	FVector2D RecoilPush;

	// Angle, in degrees, of recoil spread for the weapon. This is the max angle that RecoilPush
	// will be offset when calculating the final recoil push direction for each shot.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Recoil)
	float RecoilPushSpread;

	// If the weapon is fully automatic or single shot
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsAuto;
};

USTRUCT()
struct FWeaponAnim
{
	GENERATED_USTRUCT_BODY()

	/** Montage to play on first person mesh */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* FirstPerson = nullptr;

	/** Montage to play on third person mesh */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* ThirdPerson = nullptr;
};

UCLASS(Blueprintable, Abstract, NotPlaceable, Config = Game)
class BATTLESTAGE_API ABSWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABSWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void AttachToOwner();

	void DetachFromOwner();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = Weapon)
	virtual void ServerEquip(ABSCharacter* Character);

	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual void Unequip();

	UFUNCTION(BlueprintCallable, Category = Weapon)
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = Weapon)
	void StopFire();

	USkeletalMeshComponent* GetActiveMesh() const;

	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool CanFire() const;

	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool CanReload() const;

	UFUNCTION(BlueprintCallable, Category = Weapon)
	void Reload();

	/**
	* Get the character that owns this weapon.
	*/
	ABSCharacter* GetCharacter() const;

	/**
	* Gets the desired rotation of a shot that is fired from this weapon.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = Weapon)
	FRotator GetFireRotation() const;

	/**
	* Gets desired start position of a shot that is fired from this weapon.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = Weapon)
	FVector GetFireLocation() const;

	UFUNCTION(BlueprintCallable, Category = Weapon)
	FVector GetCameraAimLocation() const;

	UFUNCTION(BlueprintCallable, Category = Weapon)
	float GetCurrentSpread() const;

	/** AActor interface */
	virtual void PostInitProperties() override;
	virtual bool ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags) override;
	virtual void PostInitializeComponents() override;
	/** AActor interface end */

protected:
	//-----------------------------------------------------------------
	// Weapon Events
	//-----------------------------------------------------------------

	/**
	* Called after a shot has been fired by the weapon. Called on all clients when
	* notified that a shot has occurred on the server or called on local player when
	* fired locally.
	*
	* Gives the opportunity to activate visual effects or weapon stat effects
	* that should be applied following a shot.
	*/
	virtual void OnShotFired();

	/**
	* Called when the weapon state has been transition into the Firing state.
	*/
	virtual void OnEnteredFiringState();

	/**
	* Called when the weapon state has been transition out of Firing state.
	*/
	virtual void OnExitFiringState();

	/**
	* Called when the weapon state has been transition into the Equipping state.
	*/
	virtual void OnEnteredEquippingState();

	/**
	* Called when the weapon state has been transition into the Active state.
	*/
	virtual void OnEnteredActiveState();

	/**
	* Called when the weapon state has been transition into the Unequipping state.
	*/
	virtual void OnEnteredUnequippingState();

	/**
	* Called when the weapon state has been transition into the reloading state.
	*/
	virtual void OnEnteredReloadingState();

	/**
	* Called when the weapon state has been transition into the Inactive state.
	*/
	virtual void OnEnteredInactiveState();

protected:
	// Owning client only.
	void FireShot();

	void InvokeShot(const FShotData& ShotData);

	/**
	* Starts the equip sequence. Activates effects that should be played on clients, 
	* animations, sounds, etc.
	* 
	* @return The length of the sequence in seconds. If <0, OnEquipFinished will
	*			need to be called manually.
	*/
	virtual float PlayEquipSequence();

	/**
	* Starts the unequip sequence. Activates effects that should be played on clients, 
	* animations, sounds, etc.

	* @return The length of the sequence in seconds. If <0, OnUnequipFinished will
	*			need to be called manually.
	*/
	virtual float PlayUnequipSequence();

	/**
	* Plays the fire effects for the weapon only. This includes things such as muzzle flash,
	* sounds, and weapon animations.
	*/
	virtual void PlayFiringSequence();

	/**
	* Starts the reload sequence. Activates effects that should be played on clients, 
	* animations, sounds, etc.
	* 
	* @return The length of the sequence in seconds. If < 0, OnReloadFinished will
	*			need to be called manually.
	*/
	virtual float PlayReloadSequence();

	/**
	* Plays sequence that occurs when a attempt to fire that weapon has been made, but the
	* weapon is out of ammo. Activates/Deactivates effects that should be/was played on clients, 
	* animations, sounds, etc.
	*/
	virtual void PlayEmptyClipSequence();

	//-----------------------------------------------------------------
	// Weapon state transitions (Server Only)
	//-----------------------------------------------------------------

	/**
	* Attempts the set a new weapon state. The resulting state my be different
	* from the input state. Should as trying to switch to Firing while the weapon
	* clip is empty. In such case, the state will be switched to Reloading.
	* 
	* @param State	The requested new weapon state.
	*/
	UFUNCTION(BlueprintCallable, Category = Weapon)
	virtual void SetWeaponState(EWeaponState State);

	virtual void HandleNewWeaponState(const EWeaponState State);

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerHandleNewWeaponState(const EWeaponState State);

	/**
	* Server only. Called when the equip sequence has finished and
	* should be transitioned to the Active state.
	*/
	virtual void OnEquipSequenceFinished();

	/**
	* Server only. Called when the reload sequence has finished and
	* should be  transitioned to the next state, Active by default.
	* Reload data changes such as adding ammo to the clip should be done here.
	*/
	virtual void OnReloadSequenceFinished();

	/**
	* Server only. Called when the unequip sequence has finished and
	* should be transitioned to the Inactive state.
	*/
	virtual void OnUnquipSequenceFinished();

	/**
	* Server only. Called when the empty clip sequence has finished and
	* should be transitioned to the Active state.
	*/
	virtual void OnEmptyClipSequenceFinished();

private:
	/**
	* Only called on the server. Notifies the weapon that a shot has been fired
	* and to activate any unnecessary events.
	*/
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerInvokeShot(const FShotData& ShotData);

protected:
	// The character that has this weapon equipped. 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing=OnRep_BSCharacter, Category = Defaults)
	class ABSCharacter* BSCharacter = nullptr;

	// Third person mesh for the weapon
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Defaults)
	class USkeletalMeshComponent* MeshTP;

	// First person mesh for the weapon
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Defaults)
	class USkeletalMeshComponent* MeshFP;

	// Socket attachment for the muzzle location. 
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	FName MuzzleSocket;

	//-----------------------------------------------------------------
	// Weapon Firing Data
	//-----------------------------------------------------------------	
public:
	// Get the current weapon state
	UFUNCTION(BlueprintCallable, Category = Weapon)
	EWeaponState GetWeaponState() const { return WeaponState; }

	// Get remaining ammo
	UFUNCTION(BlueprintCallable, Category = Weapon)
	int32 GetRemainingAmmo() const { return RemainingAmmo; }

	// Get ammo left in clip
	UFUNCTION(BlueprintCallable, Category = Weapon)
	int32 GetRemainingClip() const { return RemainingClip; }

	const FWeaponStats& GetWeaponStats() const { return WeaponStats; }

protected:
	// The previous weapon state. This is to be used with OnRep_WeaponState
	// to respond to state changes on the client side. This should be set
	// to WeaponState on OnRep_WeaponState exit.
	UPROPERTY(BlueprintReadOnly, Category = WeaponData)
	TEnumAsByte<EWeaponState> PrevWeaponState = EWeaponState::Inactive;

	// Common weapon firing data
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponData)
	FWeaponStats WeaponStats;

	// The projectile class fired by this weapon
	UPROPERTY(EditDefaultsOnly, Category = WeaponData)
	TSubclassOf<class UBSShotType> ShotTypeClass = nullptr;

	UPROPERTY(Replicated)
	class UBSShotType* ShotType = nullptr;

	// Timer used by this server to manage weapon state changes
	// and invoke actions. Should not be used on clients.
	FTimerHandle WeaponStateTimer;

	// Timer used to to fire shots while in the Firing state for
	// automatic weapons and the FireRate.
	FTimerHandle WeaponFiringTimer;

	// Game time when the last shot was fired.
	float LastFireTime = 0.f;

	// Accumulator for recoil spread applied per shot
	float CurrentRecoilSpread = 0.f;

	// Accumulator for recoil push offset applied per shot
	FVector2D CurrentRecoilOffset = FVector2D::ZeroVector;

private:
	// Toggle flag that indicates that the server fired a shot when changed.
	// Should not be interpreted as true/false.
	UPROPERTY(ReplicatedUsing = OnRep_ServerFired)
	uint32 bServerFired : 1;

private:
	// Current state of the weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_WeaponState, Category = WeaponData, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EWeaponState> WeaponState = EWeaponState::Inactive;

	// Current ammo count
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = WeaponData, meta = (AllowPrivateAccess = "true"))
	int32 RemainingAmmo;

	// Remaining ammo left in the current clip
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = WeaponData, meta = (AllowPrivateAccess = "true"))
	int32 RemainingClip;

protected:
	//-----------------------------------------------------------------
	// Weapon Sounds
	//-----------------------------------------------------------------
	
	// Spawned component used to play FireSound and manipulate looping sounds.
	UPROPERTY(Transient)
	UAudioComponent* FireSoundComponent = nullptr;

	// Sound effect when the weapon is fire. This audio will loop while in the
	// firing weapon state if it is a looping sound. Otherwise, it will be played
	// at the time of each shot. For automatic weapons, it is recommended to use
	// looping firing sounds.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound)
	USoundBase* FireSound = nullptr;

	// Sound effect on weapon end fire
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound)
	USoundBase* EndFireSound = nullptr;

	// Sound effect on weapon fire with empty clip
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound)
	USoundBase* EmptyClipSound = nullptr;

	//-----------------------------------------------------------------
	// Weapon Animation 
	//-----------------------------------------------------------------
protected:
	UAnimMontage* GetWeaponMontage(const FWeaponAnim& WeaponAnim);

protected:
	// Played on the character on equip
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	FWeaponAnim EquipAnim;

	// Played on the character on unequip
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	FWeaponAnim UnequipAnim;

	// Played on the character on fire
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	FWeaponAnim FireAnim;

	// Camera shake on weapon fire
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	TSubclassOf<class UCameraShake> FireCameraShake;

	// Played on the character on reload
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	FWeaponAnim ReloadAnim;

	//-----------------------------------------------------------------
	// Weapon FX
	//-----------------------------------------------------------------
protected:
	// Muzzle FX for firing
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = FX)
	class UParticleSystem* MuzzleFX = nullptr;

	// Spawned particle system component for muzzle FX
	UPROPERTY(Transient)
	class UParticleSystemComponent* MuzzleFXComponent = nullptr;

private:

	//-----------------------------------------------------------------
	// On Replicated
	//-----------------------------------------------------------------
	
	UFUNCTION()
	void OnRep_ServerFired();

	/** Invokes state transition events on clients */
	UFUNCTION()
	void OnRep_WeaponState();

	UFUNCTION()
	virtual void OnRep_BSCharacter();
};

FORCEINLINE USkeletalMeshComponent* ABSWeapon::GetActiveMesh() const { return BSCharacter->IsFirstPerson() ? MeshFP : MeshTP; }

FORCEINLINE ABSCharacter* ABSWeapon::GetCharacter() const { return BSCharacter; }
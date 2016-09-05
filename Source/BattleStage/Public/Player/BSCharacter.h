// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "BSCharacter.generated.h"

class UInputComponent;
class ABSWeapon;

//-----------------------------------------------------------------
// Hit data sent to clients following a successful hit on the 
// server.
//-----------------------------------------------------------------
USTRUCT()
struct FReceiveHitInfo
{
	GENERATED_USTRUCT_BODY()

	/** Actor responsible for the hit */
	UPROPERTY(BlueprintReadOnly, Category = RecieveHitInfo)
	TWeakObjectPtr<AActor> DamageCauser;

	/** Damage that was received */
	UPROPERTY(BlueprintReadOnly, Category = RecieveHitInfo)
	float Damage;

	/** Bone that was hit */
	UPROPERTY(BlueprintReadOnly, Category = RecieveHitInfo)
	FName HitBone;

	/** Bone that was hit */
	UPROPERTY(BlueprintReadOnly, Category = RecieveHitInfo)
	FVector_NetQuantize10 HitLocation;

	/** Direction the hit came from */
	UPROPERTY(BlueprintReadOnly, Category = RecieveHitInfo)
	FVector_NetQuantizeNormal HitDirection;

	/** Forces hit info to replicate */
	void ForceReplication()
	{
		++ForceReplicateByte;
	}

private:
	UPROPERTY()
	uint8 ForceReplicateByte;
};

UENUM()
enum class EWeaponSlot : uint8
{
	Primary,
	Secondary,
	Max
};

UCLASS(config=Game)
class ABSCharacter : public ACharacter
{
	GENERATED_BODY()
private:
	static const uint32 MAX_JUMPS = 2;

public:
	ABSCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = Character)
	float GetAimSpread() const;

	float GetMovementModifier() const;

	/**
	* Enable/Disable actions on the character. (i.e. sprinting, firing weapon, etc.)
	*/
	virtual void SetDisableActions(bool bIsDisabled);

	virtual void StartFire();
	virtual void StopFire();

	UFUNCTION(BlueprintCallable, Category = Character)
	bool IsRunning() const;

	bool CanRun() const;

	virtual void SetRunning(bool bNewRunning);
	virtual void ToggleRunning();

	void ReloadWeapon();

	/** Gets info describing the last received damage hit on this character. */
	const FReceiveHitInfo& GetLastHitInfo() const;

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

	void UpdateMeshVisibility();

	UFUNCTION(BlueprintCallable, Category = Health)
	bool CanDie() const;

	UFUNCTION(BlueprintCallable, Category = Health)
	int32 GetHealth() const;				

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void SwapWeapon();

protected:
	/**
	* Called when the character dies. Base implementation plays any dying animations 
	* and sets the character mesh to ragdoll.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = Character)
	void OnDeath();

	/**
	* Called when the player is hit by a damage event, i.e. bullet, projectile, explosion, etc.
	* Responds to hit event using data in ReceiveHitInfo. Should only invoke cosmetic events,
	* animations, HUD effects, audio, etc.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = Character)
	void OnReceiveHit();

	void EnableRagdollPhysics();	

	void EquipWeapon(const EWeaponSlot WeaponSlot);

	UFUNCTION()
	virtual void OnRep_WeaponSlot();

	/** ACharacter Interface Begin */
public:	
	virtual void SetupPlayerInputComponent(class UInputComponent* InInputComponent) override;
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void Jump() override;
	virtual void OnJumped_Implementation() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void Crouch(bool bClientSimulation = false) override;
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void TurnOff() override;
protected:
	virtual bool CanJumpInternal_Implementation() const override;	
	/** ACharacter Interface End */

	/** APawn Interface Begin */
public:	
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual bool ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const override;
	virtual void PawnClientRestart() override;
	/** APawn Interface End */

	/** AActor Interface Begin */
public:
	virtual void PostInitProperties() override;
	virtual void BeginPlay() override;

	/**
	* Play Animation Montage on the character mesh. Will play on the first person mesh if
	* this is a first person character, otherwise it will be played on the third person mesh.
	*/
	virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) override;

	virtual void StopAnimMontage(class UAnimMontage* AnimMontage = NULL) override;

protected:
	virtual void OnRep_Owner() override;
	/** AActor Interface End */

protected:
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<ABSWeapon> DefaultWeapons[EWeaponSlot::Max];

	// Socket name to attach equipped weapons
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	FName WeaponEquippedSocket;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Character)
	float RunningMovementModifier;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = Character)
	uint32 bIsRunning : 1;

	UPROPERTY(EditDefaultsOnly, Category = Character)
	float CrouchCameraSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Health, Replicated)
	int32 Health;

	UPROPERTY(ReplicatedUsing = OnRep_IsDying)
	uint32 bIsDying : 1;

	UPROPERTY(BlueprintReadOnly, Transient, ReplicatedUsing = OnReceiveHit)
	FReceiveHitInfo ReceiveHitInfo;

	//-----------------------------------------------------------------
	// Animations
	//-----------------------------------------------------------------
	
	/** Animation played on death on third person mesh */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	UAnimMontage* DeathAnim = nullptr;

	/** Bone to set in RecieveHitInfo when hit with radial damage */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	FName RadialDamageImpactBone;

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEquipWeapon(const EWeaponSlot WeaponSlot);

	void OnDeathAnimEnded(UAnimMontage* Montage, bool bInterupted);

	/**
	* Server only.
	* Kills the character. Sets the character up to be killed and removes replication.
	* First person characters will be switch to third person and controllers detached.
	* 
	* @param DamageEvent		The damage event the killed the character.
	* @param EventInstigator	The event instigator.
	* @param DamageCauser		The damage causer.
	*/
	void Die(struct FDamageEvent const& DamageEvent, AController* Killer);

	void TakeHit(const float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	void UpdateViewTarget(const float DeltaSeconds);

	UFUNCTION()
	void OnRep_IsDying();

	UFUNCTION()
	void OnRep_Weapons();

	void SetReceiveHitInfo(const float Damage, FDamageEvent const& DamageEvent, AActor* Instigator);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetRunning(bool bNewRunning);

	/**
	* Server Only. 
	* Creates weapons for the character's default loadout.
	*/
	void CreateDefaultLoadout();

private:
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCamera;

	/** First person mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* FirstPersonMesh;

	UPROPERTY(ReplicatedUsing = OnRep_Weapons)
	ABSWeapon* Weapons[EWeaponSlot::Max];

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WeaponSlot, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	EWeaponSlot ActiveWeaponSlot = EWeaponSlot::Primary;

	// Eye height from last update. Used to lerp between standing and crouched camera position.
	float LastEyeHeight; 

	bool bIsActionsDisabled = false;

	uint8 JumpCounter = 0;

public:
	/** Returns FirstPersonCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }

	/** Returns FirstPersonMesh subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns ThirdPersonMesh subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetThirdPersonMesh() const { return GetMesh(); }

	/** Gets the name of the socket to attach equipped weapons. */
	FORCEINLINE FName GetWeaponEquippedSocket() const { return WeaponEquippedSocket; }

	FORCEINLINE EWeaponSlot GetActiveWeaponSlot() const { return ActiveWeaponSlot; }

	/** Gets the currently equipped weapon. Null if no weapon is equipped. */
	UFUNCTION(BlueprintCallable, Category = Mesh)
	FORCEINLINE ABSWeapon* GetEquippedWeapon() const { return Weapons[(int32)ActiveWeaponSlot]; }
};

FORCEINLINE int32 ABSCharacter::GetHealth() const
{
	return Health;
}
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BSProjectile.generated.h"

/**
* Base class for projectiles that cause radial damage, such as rockets, grenades, etc.
*
* #bstodo Should be decomposed to allow point based (i.e. Arrows) or radial
*			based (i.e. Rockets, Grenades) projectiles.
*/
UCLASS(Blueprintable, Abstract, NotPlaceable, Config = Game)
class ABSProjectile : public AActor
{
	GENERATED_BODY()

public:
	ABSProjectile(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** AActor Interface Begin */
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	/** AActor Interface End */

	/**
	* Detonates the projectile at it's current position and applies radial damage.
	* This object will be destroyed after detonation.
	*/
	UFUNCTION(BlueprintCallable, Category = Projectile)
	void Detonate();

protected:
	/**
	* Detonates the projectile at a specified position and applies radial damage.
	* This object will be destroyed after detonation.
	*/
	UFUNCTION(BlueprintCallable, Category = Projectile)
	virtual void DetonateAtLocation(const FVector& Location, const FRotator& Rotation);

	/** Called after the projectile has been detonated and is about to be destroyed. */
	UFUNCTION(BlueprintNativeEvent, Category = Projectile)
	void OnDetonate();

	UFUNCTION()
	virtual void OnImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	virtual void OnStop(const FHitResult& ImpactResult);

	/** 
	 * Called after the projectile has been detonated to allow the server to deactivate itself and allow
	 * detonation to be replicated to clients before destruction. 
	 */
	virtual void Deactivate();

	/** Blueprint hook to allow blueprint created objects to be deactivated */
	UFUNCTION(BlueprintImplementableEvent, Category = Projectile)
	void OnDeactivate();

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDetonate(const FVector& Location, const FRotator& Rotation);

	UFUNCTION()
	void OnRep_IsDetonated();

protected:
	/** Effect generated when the projectile is detonated */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
	TSubclassOf<class ABSExplosion> ExplosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
	FRadialDamageParams Damage; // #bstodo Might want to modify this based on the stats of the firing weapon

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
	TSubclassOf<class UDamageType> DamageTypeClass;

private:
	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(ReplicatedUsing = OnRep_IsDetonated)
	uint32 bIsDetonated : 1;

public:
	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }

	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};

// #bstodo Figure out weird build bug that causes undefined compile error when ABSImpactGrenade is in a separate file.
/**
*
*/
UCLASS()
class BATTLESTAGE_API ABSImpactGrenade : public ABSProjectile
{
	GENERATED_BODY()

public:
	ABSImpactGrenade(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** AActor Interface Begin */
	virtual void BeginPlay() override;
	/** AActor Interface End */

protected:
	/** ABSProjectile Interface Begin */
	virtual void OnImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	/** ABSProjectile Interface End */

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ImpactGrenade)
	float FuzeTime;
};
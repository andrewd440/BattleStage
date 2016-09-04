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
	virtual void PostActorCreated() override;
	virtual void LifeSpanExpired() override;
	/** AActor Interface End */

protected:
	/**
	* Detonates the projectile at it's current position and applies radial damage.
	* This object will be destroyed after detonation.
	*/
	UFUNCTION(BlueprintCallable, Category = Projectile)
	virtual void Detonate();

	/** Called after the projectile has been detonated and is about to be destroyed. */
	UFUNCTION(BlueprintImplementableEvent, Category = Projectile)
	void OnDetonate();

protected:
	/** Effect generated when the projectile is detonated */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
	TSubclassOf<class ABSExplosion> ExplosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
	FRadialDamageParams Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
	TSubclassOf<class UDamageType> DamageTypeClass;

private:
	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

public:
	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }

	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};


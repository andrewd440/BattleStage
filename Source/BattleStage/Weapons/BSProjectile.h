// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Actor.h"
#include "BSProjectile.generated.h"

UCLASS(Blueprintable, Abstract, NotPlaceable, Config = Game)
class ABSProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

public:
	ABSProjectile();

	/** called when projectile hits something */
	UFUNCTION()
	void OnImpact(const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }

	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	//-----------------------------------------------------------------
	// AActor Interface 
	//-----------------------------------------------------------------	
	virtual void PostActorCreated() override;
	//-----------------------------------------------------------------
	// AActor Interface End 
	//-----------------------------------------------------------------	

protected:
	// Effect generated at the surface of impact by the projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
	TSubclassOf<class UBSImpactEffect> ImpactEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
	FRadialDamageParams Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
	TSubclassOf<class UDamageType> DamageTypeClass;
};


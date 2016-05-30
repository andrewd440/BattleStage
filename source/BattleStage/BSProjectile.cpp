// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "BattleStage.h"
#include "BSProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BSImpactEffect.h"

ABSProjectile::ABSProjectile() 
{
	bReplicates = true;
	bReplicateMovement = true;

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->OnProjectileStop.AddDynamic(this, &ABSProjectile::OnImpact);

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void ABSProjectile::OnImpact(const FHitResult& Hit)
{
	if (HasAuthority())
	{
		TArray<AActor*> ToIgnore;
		ToIgnore.Add(this);

		UGameplayStatics::ApplyRadialDamageWithFalloff(this, Damage.BaseDamage, Damage.MinimumDamage, GetActorLocation(), Damage.InnerRadius, Damage.OuterRadius, Damage.DamageFalloff, DamageTypeClass, ToIgnore);

		if (ImpactEffect)
		{
			const FTransform SpawnTransform = GetTransform();

			ABSImpactEffect* SpawnedEffect = GetWorld()->SpawnActorDeferred<ABSImpactEffect>(ImpactEffect, SpawnTransform, this);
			SpawnedEffect->SurfaceHit = Hit;

			UGameplayStatics::FinishSpawningActor(SpawnedEffect, SpawnTransform);
		}

		Destroy();
	}
}
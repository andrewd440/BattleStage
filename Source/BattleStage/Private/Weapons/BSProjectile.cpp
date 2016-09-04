// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"

#include "BSExplosion.h"

ABSProjectile::ABSProjectile(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
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

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void ABSProjectile::PostActorCreated()
{
	Super::PostActorCreated();

	// Make sure we don't hit the weapon or character firing
	CollisionComp->IgnoreActorWhenMoving(GetOwner(), true);
	CollisionComp->IgnoreActorWhenMoving(GetInstigator(), true);
}

void ABSProjectile::LifeSpanExpired()
{
	Detonate();
}

void ABSProjectile::Detonate()
{
	if (ExplosionEffect)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = GetInstigator();
		SpawnParams.Owner = this;

		GetWorld()->SpawnActor<ABSExplosion>(ExplosionEffect, GetActorLocation(), GetActorRotation(), SpawnParams);
	}

	if (HasAuthority())
	{
		TArray<AActor*> ToIgnore;
		ToIgnore.Add(this);

		bool bDamagedActor = UGameplayStatics::ApplyRadialDamageWithFalloff(this,
			Damage.BaseDamage,
			Damage.MinimumDamage,
			GetActorLocation(),
			Damage.InnerRadius,
			Damage.OuterRadius,
			Damage.DamageFalloff,
			DamageTypeClass,
			ToIgnore,
			this,
			GetInstigatorController());

		OnDetonate();
		Destroy();
	}
	else
	{
		OnDetonate();
	}
}
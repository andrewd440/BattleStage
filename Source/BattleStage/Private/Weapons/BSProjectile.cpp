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
	CollisionComp->SetNotifyRigidBodyCollision(true);
	CollisionComp->bGenerateOverlapEvents = false;

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

	bIsDetonated = false;
}

void ABSProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	CollisionComp->OnComponentHit.AddDynamic(this, &ABSProjectile::OnImpact);

	ProjectileMovement->OnProjectileStop.RemoveAll(this);
	ProjectileMovement->OnProjectileStop.AddDynamic(this, &ABSProjectile::OnStop);

	// Make sure we don't hit the weapon or character firing
	CollisionComp->IgnoreActorWhenMoving(GetOwner(), true);
	CollisionComp->IgnoreActorWhenMoving(GetInstigator(), true);
}

void ABSProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(ABSProjectile, bIsDetonated);
}

void ABSProjectile::DetonateAtLocation(const FVector& Location, const FRotator& Rotation)
{
	if (!bIsDetonated)
	{
		if (!HasAuthority())
		{
			ServerDetonate(Location, Rotation);
		}
		else
		{
			TArray<AActor*> ToIgnore;
			ToIgnore.Add(this);

			bool bDamagedActor = UGameplayStatics::ApplyRadialDamageWithFalloff(this,
				Damage.BaseDamage,
				Damage.MinimumDamage,
				Location,
				Damage.InnerRadius,
				Damage.OuterRadius,
				Damage.DamageFalloff,
				DamageTypeClass,
				ToIgnore,
				this,
				GetInstigatorController());

			bIsDetonated = true;
			OnDetonate();

			Deactivate();
			TearOff();
		}
	}
}

void ABSProjectile::Detonate()
{
	DetonateAtLocation(GetActorLocation(), GetActorRotation());
}

void ABSProjectile::OnImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}

void ABSProjectile::OnStop(const FHitResult& ImpactResult)
{

}

void ABSProjectile::Deactivate()
{
	if (!IsPendingKillPending())
	{
		SetActorEnableCollision(false);
		ProjectileMovement->SetActive(false);

		OnDeactivate();
		SetLifeSpan(0.5f); // Allow replication time to clients for detonation
	}

	bIsDetonated = true;
}

void ABSProjectile::ServerDetonate_Implementation(const FVector& Location, const FRotator& Rotation)
{
	DetonateAtLocation(Location, Rotation);
}

bool ABSProjectile::ServerDetonate_Validate(const FVector&, const FRotator&)
{
	return true;
}

void ABSProjectile::OnDetonate_Implementation()
{
	if (ExplosionEffect)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = GetInstigator();
		SpawnParams.Owner = this;

		GetWorld()->SpawnActor<ABSExplosion>(ExplosionEffect, GetActorLocation(), GetActorRotation(), SpawnParams);
	}
}

void ABSProjectile::OnRep_IsDetonated()
{
	OnDetonate();
	Destroy();
}

ABSImpactGrenade::ABSImpactGrenade(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	FuzeTime = 2.f;
}

void ABSImpactGrenade::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() && FuzeTime > 0.f)
	{
		FTimerHandle FuzeHandle;
		GetWorld()->GetTimerManager().SetTimer(FuzeHandle, this, &ABSImpactGrenade::Detonate, FuzeTime);
	}
}

void ABSImpactGrenade::OnImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (HasAuthority() && Cast<ABSCharacter>(OtherActor))
	{
		const FVector NudgedLocation = Hit.ImpactPoint + Hit.ImpactNormal * 10.f; // Make sure we don't detonate inside a surface
		DetonateAtLocation(NudgedLocation, GetActorRotation());
	}
}

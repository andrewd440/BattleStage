// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"

#include "BSInstantShot.h"
#include "BSWeapon.h"
#include "BSImpactEffect.h"

static const float MAX_SHOT_RANGE = 10000.f;

void UBSInstantShot::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UBSInstantShot, ShotRep, COND_SkipOwner);
}

bool UBSInstantShot::GetShotData(FShotData& OutShotData) const
{
	const ABSWeapon* const Weapon = GetWeapon();
	OutShotData.Start = Weapon->GetAimLocation();

	// Get a random weapon spread for the shot
	const int32 RandomSeed = FMath::Rand();
	FRandomStream SpreadStream(RandomSeed);
	const float BaseSpread = FMath::DegreesToRadians(Weapon->GetCurrentSpread());

	// Use spread to offset shot
	const FVector TrueAimDirection = Weapon->GetAimRotation().Vector();
	OutShotData.Direction = SpreadStream.VRandCone(TrueAimDirection, BaseSpread);

	const FVector FireEnd = OutShotData.Start + OutShotData.Direction * MAX_SHOT_RANGE;

	OutShotData.Impact = WeaponTrace(OutShotData.Start, FireEnd);
	OutShotData.bImpactNeeded = OutShotData.Impact.bBlockingHit ? true : false;

	return true;
}

void UBSInstantShot::PreInvokeShot(const FShotData& ShotData)
{
	// Play hit locally if this is not the server
	const ABSWeapon* const Weapon = GetWeapon();
	if (!Weapon->HasAuthority())
	{
		const FVector Target = ShotData.Start + ShotData.Direction * MAX_SHOT_RANGE;
		SimulateFire(Target);
	}
}

void UBSInstantShot::InvokeShot(const FShotData& ShotData)
{
	if (ShotData.bImpactNeeded)
	{
		ProcessHit(ShotData);
	}
	else
	{		
		ProcessMiss(ShotData);
	}	
}

void UBSInstantShot::PlayTrailEffects(const FVector& End) const
{
	if (TrailFX)
	{	
		const ABSWeapon* const Weapon = GetWeapon();

		const FVector Start = Weapon->GetFireLocation();
		const FVector DirectionVector = (End - Start);

		UParticleSystemComponent* TrailComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TrailFX, Start, DirectionVector.GetUnsafeNormal().Rotation());
		if (TrailEndParam != NAME_None)
		{
			TrailComponent->SetFloatParameter(TrailEndParam, DirectionVector.Size());
		}
	}
}

void UBSInstantShot::PlayImpactEffects(const FHitResult& Hit) const
{
	if (ImpactEffect)
	{
		const UBSImpactEffect* const EffectObject = ImpactEffect->GetDefaultObject<UBSImpactEffect>();
		EffectObject->SpawnEffect(GetWorld(), Hit);
	}
}

void UBSInstantShot::RespondValidatedShot(const FShotData& ShotData)
{
	ABSWeapon* const Weapon = GetWeapon();
	const FVector ShotEnd = (ShotData.bImpactNeeded) ? ShotData.Impact.ImpactPoint : ShotData.Start + ShotData.Direction * MAX_SHOT_RANGE;

	if (Weapon->HasAuthority())
	{
		if (ShotData.bImpactNeeded && ShotData.Impact.Actor.IsValid())
		{
			AActor& HitActor = *ShotData.Impact.Actor;

			const float BaseDamage = Weapon->GetWeaponStats().BaseDamage;
			const FPointDamageEvent DamageEvent(BaseDamage, ShotData.Impact, -ShotData.Direction, DamageType);

			HitActor.TakeDamage(BaseDamage, DamageEvent, Weapon->GetInstigatorController(), Weapon->GetCharacter());
		}

		// Simulate on remotes
		ShotRep.Target = ShotEnd;
		ShotRep.FireToggle = !ShotRep.FireToggle;
	}

	// Play local effects
	if (Weapon->GetNetMode() != NM_DedicatedServer)
	{
		if (ShotData.Impact.bBlockingHit)
			PlayImpactEffects(ShotData.Impact);

		PlayTrailEffects(ShotEnd);
	}
}

void UBSInstantShot::SimulateFire(const FVector& Target) const
{
	ABSWeapon* const Weapon = GetWeapon();

	const FVector AimStart = Weapon->GetAimLocation();

	// Trace in target direction to prevent missing the target by small amounts when simulating a 
	// replicated shot that hit a target.
	const FVector TraceEnd = AimStart + (Target - AimStart).GetSafeNormal() * MAX_SHOT_RANGE;
	const FHitResult Impact = WeaponTrace(AimStart, TraceEnd);

	if (Impact.bBlockingHit)
		PlayImpactEffects(Impact);

	PlayTrailEffects(Impact.bBlockingHit ? Impact.ImpactPoint : Impact.TraceEnd);
}

FHitResult UBSInstantShot::WeaponTrace(const FVector& Start, const FVector& End) const
{
	ABSWeapon* const Weapon = GetWeapon();

	FHitResult Impact;
	FCollisionQueryParams QueryParams(NAME_None, false, Weapon);
	QueryParams.bReturnPhysicalMaterial = true;
	GetWorld()->LineTraceSingleByChannel(Impact, Start, End, WEAPON_CHANNEL, QueryParams);

	return Impact;
	
}

void UBSInstantShot::OnRep_ShotRep()
{
	SimulateFire(ShotRep.Target);
}

void UBSInstantShot::ProcessHit(const FShotData& ShotData)
{
	// #bstodo Will need to verify a valid hit on the server
	RespondValidatedShot(ShotData);
}

void UBSInstantShot::ProcessMiss(const FShotData& ShotData)
{
	RespondValidatedShot(ShotData);
}

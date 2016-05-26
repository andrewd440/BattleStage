// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BSImpactEffect.generated.h"

UCLASS()
class BATTLESTAGE_API ABSImpactEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABSImpactEffect();

	// #bstodo Need to create effects made in derived blueprints

	//void SpawnEffect(AActor* Instigator, const FVector& Location, EReplicationOption ReplicationOption);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
	class USoundBase* ImpactSound;
};

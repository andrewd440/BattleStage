// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "BSNetworkUtils.generated.h"

class AActor;
class USoundBase;

/** Replication options used for UBSNetworkUtils parameters.*/
UENUM(BlueprintType)
enum class EReplicationOption : uint8
{
	All, // Replicate to all clients
	LocalOnly, // Only play locally
	AllButOWner, // Replicate to all clients but owner
};

/**
 * Utility library for network based operations.
 */
UCLASS()
class BATTLESTAGE_API UBSNetworkUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	* Plays a sound in the world with replication options.
	* 
	* @param Sound - The sound to play.
	* @param SourceActor - The actor that is the source of the sound.
	* @param SoundLocation - The location to play the sound. If zero, the sound will be attached to the source actor.
	* @param ReplicationOption - The replication option for playing the sound.
	*/
	UFUNCTION(BlueprintCallable, Category = Network)
	static void PlaySound(USoundBase* Sound, AActor* SourceActor, const FVector& SoundLocation, const EReplicationOption ReplicationOption);
};

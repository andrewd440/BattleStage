// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "BSNetworkUtils.h"
#include "BSPlayerController.h"

void UBSNetworkUtils::PlaySound(USoundBase* Sound, AActor* SourceActor, const FVector& SoundLocation, const EReplicationOption ReplicationOption)
{
	if (!Sound)
	{
		UE_LOG(BattleStage, Warning, TEXT("UBSNetworkUtils::PlaySound Sound parameter is null."));
	}
	else if (!SourceActor)
	{
		UE_LOG(BattleStage, Warning, TEXT("UBSNetworkUtils::PlaySound SourceActor parameter is null."));
	}
	else if (!SourceActor->GetWorld())
	{
		UE_LOG(BattleStage, Warning, TEXT("UBSNetworkUtils::PlaySound SourceActor does not have a world."));
	}
	else
	{
		UWorld* const World = SourceActor->GetWorld();

		const UNetDriver* const NetDriver = World->GetNetDriver();
		const ENetMode NetMode = World->GetNetMode();
		const ABSPlayerController* SourceOwner = nullptr;
		
		for (const AActor* TestOwner = SourceActor; TestOwner && !SourceOwner; TestOwner = TestOwner->GetOwner())
		{
			SourceOwner = Cast<ABSPlayerController>(TestOwner);
		}

		if (NetMode != NM_Client && NetMode != NM_Standalone && ReplicationOption != EReplicationOption::LocalOnly)
		{
			// Replicate to clients
			for (int32 i = 0; i < NetDriver->ClientConnections.Num(); ++i)
			{
				ABSPlayerController* PlayerController = Cast<ABSPlayerController>(NetDriver->ClientConnections[i]->OwningActor);

				if(ReplicationOption == EReplicationOption::AllButOWner && PlayerController == SourceOwner)
					continue;

				PlayerController->ClientHearSound(Sound, SourceActor, SoundLocation);
			}
		}

		// Play for local players
		for (FLocalPlayerIterator Itr(GEngine, World); Itr; ++Itr)
		{
			const ABSPlayerController* PlayController = Cast<ABSPlayerController>(Itr->GetPlayerController(World));
			if (PlayController)
			{
				PlayController->ClientHearSound(Sound, SourceActor, SoundLocation);
			}
		}
	}
}

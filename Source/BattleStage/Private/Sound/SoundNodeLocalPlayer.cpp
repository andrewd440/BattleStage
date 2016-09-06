// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleStage.h"
#include "SoundNodeLocalPlayer.h"
#include "ActiveSound.h"



#define LOCTEXT_NAMESPACE "BattleStage.SoundNodeLocalPlayer"

#if WITH_EDITOR
FText USoundNodeLocalPlayer::GetInputPinName(int32 PinIndex) const
{
	return (PinIndex == 0) ? LOCTEXT("LocalPlayer", "Local") : LOCTEXT("RemotePlayer", "Remote");
}
#endif 

int32 USoundNodeLocalPlayer::GetMaxChildNodes() const
{
	return 2;
}

int32 USoundNodeLocalPlayer::GetMinChildNodes() const
{
	return 2;
}

void USoundNodeLocalPlayer::ParseNodes(FAudioDevice* AudioDevice, const UPTRINT NodeWaveInstanceHash, FActiveSound& ActiveSound, const struct FSoundParseParameters& ParseParams, TArray<FWaveInstance *>& WaveInstances)
{
	check(IsInGameThread());

	UAudioComponent* const AudioComponent = ActiveSound.GetAudioComponent();
	AActor* AudioOwner = AudioComponent ? AudioComponent->GetOwner() : nullptr;

	// Walk up the owner chain until we get a controller
	APlayerController* AudioController = Cast<APlayerController>(AudioOwner);
	while (AudioOwner && AudioController == nullptr)
	{
		AudioOwner = AudioOwner->GetOwner();
		AudioController = Cast<APlayerController>(AudioOwner);
	}

	bool bIsLocal = AudioController ? AudioController->IsLocalController() : false;
	int32 PlayIndex = bIsLocal ? 0 : 1;

	if (PlayIndex < ChildNodes.Num() && ChildNodes[PlayIndex])
	{
		ChildNodes[PlayIndex]->ParseNodes(AudioDevice, GetNodeWaveInstanceHash(NodeWaveInstanceHash, ChildNodes[PlayIndex], PlayIndex), ActiveSound, ParseParams, WaveInstances);
	}
}

#undef LOCTEXT_NAMESPACE
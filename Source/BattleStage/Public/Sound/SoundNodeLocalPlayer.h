// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Sound/SoundNode.h"
#include "SoundNodeLocalPlayer.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESTAGE_API USoundNodeLocalPlayer : public USoundNode
{
	GENERATED_BODY()
	
public:
	/** USoundNode Interface Begin */
#if WITH_EDITOR
	virtual FText GetInputPinName(int32 PinIndex) const override;
#endif

	virtual int32 GetMaxChildNodes() const override;
	virtual int32 GetMinChildNodes() const override;
	virtual void ParseNodes(FAudioDevice* AudioDevice, const UPTRINT NodeWaveInstanceHash, FActiveSound& ActiveSound, const struct FSoundParseParameters& ParseParams, TArray<FWaveInstance *>& WaveInstances) override;
	/** USoundNode Interface End */
};

// Copyright 2020-2021 Jason Ioffe and Di Ioffe


#pragma once

#include "UObject/Class.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "FOneShotEffect.generated.h"

USTRUCT(BlueprintType)
struct FOneShotEffect
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundCue* Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UParticleSystem* ParticleEffect;

	FOneShotEffect()
	{
		Sound = nullptr;
		ParticleEffect = nullptr;
	}
};
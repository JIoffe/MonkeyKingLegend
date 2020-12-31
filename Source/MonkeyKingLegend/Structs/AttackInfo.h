// Copyright 2020-2021 Jason Ioffe and Di Ioffe

#pragma once

#include "UObject/Class.h"
#include "../Enums/AttackType.h"
#include "../FX/FOneShotEffect.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "AttackInfo.generated.h"

USTRUCT(BlueprintType)
struct FAttackInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<EAttackType> AttackType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BaseDamageMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* AnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundCue* AttackSoundCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FOneShotEffect StrikeWorldEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FOneShotEffect StrikeEnemyEffect;

	FAttackInfo()
	{
		AttackType = EAttackType::AT_Physical;
		BaseDamageMultiplier = 1.0f;
		AnimMontage = nullptr;
		AttackSoundCue = nullptr;

		StrikeWorldEffect = {};
		StrikeEnemyEffect = {};
	}
};
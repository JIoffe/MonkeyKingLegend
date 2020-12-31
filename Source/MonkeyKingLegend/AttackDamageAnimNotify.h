// Copyright 2020-2021 Jason Ioffe and Di Ioffe

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AttackDamageAnimNotify.generated.h"

/**
 * 
 */
UCLASS()
class MONKEYKINGLEGEND_API UAttackDamageAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};

// Copyright 2020-2021 Jason Ioffe and Di Ioffe

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MonkeyKingMovementComponent.generated.h"

/**
 * Character movement specific to Sun Wukong
 */
UCLASS()
class MONKEYKINGLEGEND_API UMonkeyKingMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	UMonkeyKingMovementComponent(const FObjectInitializer& ObjectInitializer);

	//Overrides
	//void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
};
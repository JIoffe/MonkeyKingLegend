// Copyright 2020-2021 Jason Ioffe and Di Ioffe


#include "MonkeyKingMovementComponent.h"

UMonkeyKingMovementComponent::UMonkeyKingMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bOrientRotationToMovement = true;
	bUseControllerDesiredRotation = true;

	//Some default settings that make sense for the monkey king
	GravityScale = 2.75f;
	JumpZVelocity = 1000.0f;
	AirControl = 0.95f;
}
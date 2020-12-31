#pragma once
#include "UObject/Class.h"

UENUM(BlueprintType)
enum class EMovementState : uint8 {
	Normal				UMETA(DisplayName = "Normal"),
	SlidingDownSlope	UMETA(DisplayName = "Sliding Down Slope"),
	BoundByAction		UMETA(DisplayName = "Bound By Action"),
	BoundByEffect		UMETA(DisplayName = "Bound By Effect")
};
#pragma once
#include "UObject/Class.h"

UENUM(BlueprintType)
enum class EAttackState : uint8 {
	None				UMETA(DisplayName = "None"),
	Primary				UMETA(DisplayName = "Primary"),
	Secondary			UMETA(DisplayName = "Secondary"),
	PrimaryBuffed		UMETA(DisplayName = "PrimaryBuffed"),
	SecondaryBuffed     UMETA(DisplayName = "SecondaryBuffed"),
	PrimaryAir          UMETA(DisplayName = "PrimaryAir"),
	SecondaryAir		UMETA(DisplayName = "SecondaryAir")
};
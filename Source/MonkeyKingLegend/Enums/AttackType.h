#pragma once
#include "UObject/Class.h"

UENUM(BlueprintType)
enum class EAttackType : uint8 {
    AT_Physical                 UMETA(DisplayName = "Physical"),
    AT_PhysicalKnockback        UMETA(DisplayName = "PhysicalKnockback")
};
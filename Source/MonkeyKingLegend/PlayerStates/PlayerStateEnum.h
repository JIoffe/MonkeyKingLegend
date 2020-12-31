#pragma once
#include "UObject/Class.h"

UENUM(BlueprintType)
enum class EPlayerState : uint8 {
    CoreLocomotion          UMETA(DisplayName = "CoreLocomotion"),
    PrimaryAttackCombo      UMETA(DisplayName = "PrimaryAttackCombo"),
    SpecialAttackReady      UMETA(DisplayName = "SpecialAttackReady"),
    Sprinting               UMETA(DisplayName = "Sprinting"),
    Dodging                 UMETA(DisplayName = "Dodging"),

    None                    UMETA(DisplayName = "None")
};
#pragma once

#include "CoreMinimal.h"
#include "WeaponEnums.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    Unarmed UMETA(DisplayName = "Unarmed"),
    OneHandSword UMETA(DisplayName = "1H Sword"),
    TwoHandSword UMETA(DisplayName = "2H Sword"),
    OneHandAxe UMETA(DisplayName = "1H Axe"),
    TwoHandAxe UMETA(DisplayName = "2H Axe"),
};
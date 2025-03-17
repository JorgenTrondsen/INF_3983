#pragma once

#include "CoreMinimal.h"
#include "CharacterEnums.generated.h"

UENUM(BlueprintType)
enum class ECharacterRace : uint8
{
    Race1 UMETA(DisplayName = "Race 1"),
    Race2 UMETA(DisplayName = "Race 2"),
    Race3 UMETA(DisplayName = "Race 3"),
    Race4 UMETA(DisplayName = "Race 4")
};

UENUM(BlueprintType)
enum class ECharacterGender : uint8
{
    Male UMETA(DisplayName = "Male"),
    Female UMETA(DisplayName = "Female")
};
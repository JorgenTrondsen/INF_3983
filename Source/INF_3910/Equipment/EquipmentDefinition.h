#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "EquipmentDefinition.generated.h"

/**
 *
 */
UCLASS(BlueprintType, Blueprintable)
class INF_3910_API UEquipmentDefinition : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Info")
    FGameplayTag ItemTag;

    UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Info")
    FGameplayTagContainer SlotTags;

    UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Stats")
    FGameplayTagContainer PossibleStatRolls;

    UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Ability")
    FGameplayTag AbilityTag;
};
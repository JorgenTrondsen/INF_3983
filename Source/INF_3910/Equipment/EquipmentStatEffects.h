#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "EquipmentStatEffects.generated.h"

/**
 *
 */
UCLASS()
class INF_3910_API UEquipmentStatEffects : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly)
    TMap<FGameplayTag, TObjectPtr<UDataTable>> MasterStatMap;
};
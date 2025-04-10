#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EquipmentInstance.generated.h"

/**
 *
 */
UCLASS()
class INF_3910_API UEquipmentInstance : public UObject
{
    GENERATED_BODY()

public:
    virtual void OnEquipped();
    virtual void OnUnEquipped();
};
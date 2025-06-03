#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventoryInterface.generated.h"

class UInventoryComponent;

UINTERFACE(MinimalAPI)
class UInventoryInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 *
 */
class INF_3910_API IInventoryInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    UInventoryComponent *GetInventoryComponent();
};
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractableInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * Interface for actors that can be interacted with
 */
class INF_3910_API IInteractableInterface
{
    GENERATED_BODY()

public:
    // Called when an actor interacts with this object
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void OnInteract(AActor *InteractingActor);
};

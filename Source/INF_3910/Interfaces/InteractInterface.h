#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * Interface for actors that can be interacted with
 */
class INF_3910_API IInteractInterface
{
    GENERATED_BODY()

public:
    // Add interface functions to this class. This is the class that will be inherited to implement this interface.
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void BeginInteract(APlayerController *InteractingController);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void EndInteract();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void TriggerInteract(APlayerController *InteractingController);
};

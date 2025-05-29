#pragma once

#include "CoreMinimal.h"
#include "INF_3910/AbilitySystem/Abilities/INFGameplayAbility.h"
#include "InteractAbility.generated.h"

class AItemActor;
class UInventoryComponent;

/**
 *
 */
UCLASS()
class INF_3910_API UInteractAbility : public UINFGameplayAbility
{
    GENERATED_BODY()

public:
    UInteractAbility();

    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void PerformInteraction();

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Interaction")
    float InteractionDistance;

private:
    AItemActor *PerformLineTrace(AActor *OwningActor);
    UInventoryComponent *GetInventoryComponentFromActor(AActor *Actor);
};

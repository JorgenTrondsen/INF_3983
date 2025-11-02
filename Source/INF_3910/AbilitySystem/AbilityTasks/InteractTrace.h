#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "InteractTrace.generated.h"

class IInteractInterface;
/**
 *
 */
UCLASS()
class INF_3910_API UInteractTrace : public UAbilityTask
{
    GENERATED_BODY()

public:
    UInteractTrace();

    UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "OwningAbility", HidePin = "OwningAbility", BlueprintInternalUseOnly = true))
    static UInteractTrace *InteractTrace(UGameplayAbility *OwningAbility);

    virtual void Activate() override;
    virtual void TickTask(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "InteractTrace")
    void TriggerInteract();

private:
    UPROPERTY()
    TObjectPtr<APlayerController> PlayerController;

    FHitResult InteractTraceHit;

    TScriptInterface<IInteractInterface> ThisFrameInteractActor;
    TScriptInterface<IInteractInterface> LastFrameInteractActor;
};
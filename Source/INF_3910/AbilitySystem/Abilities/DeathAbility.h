#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DeathAbility.generated.h"

/**
 *
 */
UCLASS()
class INF_3910_API UDeathAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UDeathAbility();

protected:
    UFUNCTION(BlueprintCallable, Category = "Death")
    void KillPlayer();

    UFUNCTION(BlueprintCallable, Category = "Death")
    void RespawnPlayer();
};

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Animation/AnimInstance.h"
#include "INFAnimInstance.generated.h"

/**
 *
 */
UCLASS()
class INF_3910_API UINFAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    void InitializeWithAbilitySystem(UAbilitySystemComponent *ASC);

private:
    UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Property Map")
    FGameplayTagBlueprintPropertyMap PropertyMap;
};
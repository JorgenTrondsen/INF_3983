#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "INFGameplayAbility.generated.h"

class UINFAbilitySystemComponent;

/**
 *
 */
UCLASS()
class INF_3910_API UINFGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Input")
	FGameplayTag InputTag;

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilitySpec &Spec) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	TObjectPtr<UINFAbilitySystemComponent> OwningASC;
};

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "INFGameplayAbility.generated.h"

class UINFAbilitySystemComponent;
class AINFPlayerState;
class UAnimData;

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

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Multiplayer")
	bool bIsClientPassive = false;

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilitySpec &Spec) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	TObjectPtr<UINFAbilitySystemComponent> OwningASC;

	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	TObjectPtr<AINFPlayerState> OwningPlayerState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
	TObjectPtr<UAnimData> AnimDataAsset;
};

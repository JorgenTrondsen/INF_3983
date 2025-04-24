#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "INFAbilitySystemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UINFAbilitySystemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class INF_3910_API IINFAbilitySystemInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	USceneComponent *GetDynamicSpawnPoint();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetDynamicProjectile(const FGameplayTag &ProjectileTag);
};
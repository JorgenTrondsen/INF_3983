// INF_3910 by Jørgen Trondsen, Marcus Ryan and Adrian Moen

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "INFInputConfig.generated.h"

class UInputAction;

USTRUCT()
struct FINFInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag = FGameplayTag();
};

/**
 *
 */
UCLASS()
class INF_3910_API UINFInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FINFInputAction> INFInputActions;
};

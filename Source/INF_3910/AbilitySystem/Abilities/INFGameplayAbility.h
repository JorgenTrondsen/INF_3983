// INF_3910 by Jørgen Trondsen, Marcus Ryan and Adrian Moen

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "INFGameplayAbility.generated.h"

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
};


/*
// INF_3910 by Jørgen Trondsen, Marcus Ryan and Adrian Moen

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GASGameplayAbility.generated.h"
 UCLASS()
 class INF_3910_API UGASGameplayAbility : public UGameplayAbility
 {
	 GENERATED_BODY()
 
 public:
	 UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Input")
	 FGameplayTag InputTag;
 };
 
*/

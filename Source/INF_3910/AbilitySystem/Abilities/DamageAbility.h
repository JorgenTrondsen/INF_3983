#pragma once

#include "CoreMinimal.h"
#include "INFGameplayAbility.h"
#include "DamageAbility.generated.h"

struct FDamageEffectInfo;
/**
 *
 */
UCLASS()
class INF_3910_API UDamageAbility : public UINFGameplayAbility
{
	GENERATED_BODY()

public:
	void CaptureDamageEffectInfo(AActor *TargetActor, FDamageEffectInfo &OutInfo);

	UPROPERTY()
	float BaseDamage = 0.f;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Damage Effect")
	TSubclassOf<UGameplayEffect> DamageEffect;
};
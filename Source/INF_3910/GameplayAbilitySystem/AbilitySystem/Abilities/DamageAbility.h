// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GASGameplayAbility.h"
#include "DamageAbility.generated.h"

struct FDamageEffectInfo;
/**
 * 
 */
UCLASS()
class INF_3910_API UDamageAbility : public UGASGameplayAbility
{
	GENERATED_BODY()

public:

	void CaptureDamageEffectInfo(AActor* TargetActor, FDamageEffectInfo& OutInfo);

private:

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Damage Effect")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Damage Effect")
	FScalableFloat BaseDamage;
};
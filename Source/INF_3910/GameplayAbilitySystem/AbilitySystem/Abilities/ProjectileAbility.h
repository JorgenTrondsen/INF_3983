// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageAbility.h"
#include "INF_3910/GameplayAbilitySystem/AbilitySystem/AbilityTypes.h"
#include "INF_3910/GameplayAbilitySystem/AbilitySystem/Abilities/INFGameplayAbility.h"
#include "ProjectileAbility.generated.h"

/**
 *
 */
UCLASS()
class INF_3910_API UProjectileAbility : public UDamageAbility
{
	GENERATED_BODY()

public:
	UProjectileAbility();

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilitySpec &Spec) override;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Projectile")
	FGameplayTag ProjectileToSpawnTag;

private:
	UPROPERTY()
	TObjectPtr<AActor> AvatarActorFromInfo;

	FProjectileParams CurrentProjectileParams;

	UFUNCTION(BlueprintCallable)
	void SpawnProjectile();
};
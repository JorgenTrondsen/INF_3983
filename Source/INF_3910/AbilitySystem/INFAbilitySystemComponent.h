// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "INFAbilitySystemComponent.generated.h"

/**
 *
 */
UCLASS()
class INF_3910_API UINFAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void AddCharacterAbilities(const TArray<TSubclassOf<class UGameplayAbility>> &AbilitiesToGrant);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<class UGameplayAbility>> &PassivesToGrant);
	void InitializeDefaultAttributes(const TSubclassOf<UGameplayEffect> &AttributesEffect);

	void AbilityInputPressed(FGameplayTag InputTag);
	void AbilityInputReleased(FGameplayTag InputTag);

	void SetDynamicProjectile(const FGameplayTag &ProjectileTag);

private:
	FGameplayAbilitySpecHandle ActiveProjectileAbility;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Projectile Ability")
	TSubclassOf<UGameplayAbility> DynamicProjectileAbility;

	UFUNCTION(Server, Reliable)
	void ServerSetDynamicProjectile(const FGameplayTag &ProjectileTag);
};

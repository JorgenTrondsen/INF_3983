// Fill out your copyright notice in the Description page of Project Settings.


#include "GASAbilitySystemComponent.h"



void UGASAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<class UGameplayAbility>>& AbilitiesToGrant)
{
    for (const TSubclassOf<UGameplayAbility> &Ability : AbilitiesToGrant)
    {
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1.0f);
        GiveAbility(AbilitySpec);
    }
}

void UGASAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<class UGameplayAbility>>& PassivesToGrant)
{
    for (const TSubclassOf<UGameplayAbility> &Ability : PassivesToGrant)
    {
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1.0f);
        GiveAbilityAndActivateOnce(AbilitySpec);
    } 
}

void UGASAbilitySystemComponent::InitializeDefaultAttributes(const TSubclassOf<UGameplayEffect>& AttributeEffect)
{
    checkf(AttributeEffect, TEXT("No valid default attributes for this characer %s"), *GetAvatarActor()->GetName());

    const FGameplayEffectContextHandle ContextHandle = MakeEffectContext();
    const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(AttributeEffect, 1.0f, ContextHandle);
    ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

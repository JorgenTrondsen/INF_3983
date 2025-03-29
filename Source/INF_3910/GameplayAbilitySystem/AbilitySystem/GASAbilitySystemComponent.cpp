// Fill out your copyright notice in the Description page of Project Settings.

#include "GASAbilitySystemComponent.h"
#include "INF_3910/GameplayAbilitySystem/AbilitySystem/Abilities/ProjectileAbility.h"
#include "INF_3910/GameplayAbilitySystem/AbilitySystem/Abilities/GASGameplayAbility.h"


void UGASAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<class UGameplayAbility>> &AbilitiesToGrant)
{
    for (const TSubclassOf<UGameplayAbility> &Ability : AbilitiesToGrant)
    {
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1.f);

        if (const UGASGameplayAbility *GASAbility = Cast<UGASGameplayAbility>(AbilitySpec.Ability))
        {
            AbilitySpec.DynamicAbilityTags.AddTag(GASAbility->InputTag);
            GiveAbility(AbilitySpec);
        }
    }
}

void UGASAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<class UGameplayAbility>> &PassivesToGrant)
{
    for (const TSubclassOf<UGameplayAbility> &Ability : PassivesToGrant)
    {
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1.0f);
        GiveAbilityAndActivateOnce(AbilitySpec);
    }
}

void UGASAbilitySystemComponent::InitializeDefaultAttributes(const TSubclassOf<UGameplayEffect> &AttributeEffect)
{
    checkf(AttributeEffect, TEXT("No valid default attributes for this characer %s"), *GetAvatarActor()->GetName());

    const FGameplayEffectContextHandle ContextHandle = MakeEffectContext();
    const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(AttributeEffect, 1.0f, ContextHandle);
    ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void UGASAbilitySystemComponent::AbilityInputPressed(FGameplayTag InputTag)
{
    if (!InputTag.IsValid())
        return;

    ABILITYLIST_SCOPE_LOCK();
    for (const FGameplayAbilitySpec &Spec : GetActivatableAbilities())
    {
        if (Spec.DynamicAbilityTags.HasTagExact(InputTag))
        {
            if (!Spec.IsActive())
            {
                TryActivateAbility(Spec.Handle);
            }
            else
            {
                InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
            }
        }
    }
}

void UGASAbilitySystemComponent::AbilityInputReleased(FGameplayTag InputTag)
{
    if (!InputTag.IsValid())
        return;

    ABILITYLIST_SCOPE_LOCK();
    for (const FGameplayAbilitySpec &Spec : GetActivatableAbilities())
    {
        if (Spec.DynamicAbilityTags.HasTagExact(InputTag))
        {
            InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
        }
    }
}

void UGASAbilitySystemComponent::SetDynamicProjectile(const FGameplayTag& ProjectileTag)
{
	if (!ProjectileTag.IsValid()) return;
	
	if (!GetAvatarActor()->HasAuthority())
	{
		ServerSetDynamicProjectile(ProjectileTag);
		return;
	}

	if (ActiveProjectileAbility.IsValid())
	{
		ClearAbility(ActiveProjectileAbility);
	}

	if (IsValid(DynamicProjectileAbility))
	{
		FGameplayAbilitySpec Spec = FGameplayAbilitySpec(DynamicProjectileAbility, 1);
		if (UProjectileAbility* ProjectileAbility = Cast<UProjectileAbility>(Spec.Ability))
		{
			ProjectileAbility->ProjectileToSpawnTag = ProjectileTag;
			Spec.DynamicAbilityTags.AddTag(ProjectileAbility->InputTag);

			ActiveProjectileAbility = GiveAbility(Spec);
		}
	}
	
}

void UGASAbilitySystemComponent::ServerSetDynamicProjectile_Implementation(const FGameplayTag& ProjectileTag)
{
	SetDynamicProjectile(ProjectileTag);
}
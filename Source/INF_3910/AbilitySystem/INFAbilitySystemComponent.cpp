// Fill out your copyright notice in the Description page of Project Settings.

#include "INFAbilitySystemComponent.h"
#include "INF_3910/AbilitySystem/Abilities/ProjectileAbility.h"
#include "INF_3910/AbilitySystem/Abilities/INFGameplayAbility.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "INF_3910/Equipment/EquipmentManagerComponent.h"

void UINFAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<class UGameplayAbility>> &AbilitiesToGrant)
{
    for (const TSubclassOf<UGameplayAbility> &Ability : AbilitiesToGrant)
    {
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1.f);

        if (const UINFGameplayAbility *INFAbility = Cast<UINFGameplayAbility>(AbilitySpec.Ability))
        {
            AbilitySpec.DynamicAbilityTags.AddTag(INFAbility->InputTag);
            GiveAbility(AbilitySpec);
        }
    }
}

void UINFAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<class UGameplayAbility>> &PassivesToGrant)
{
    for (const TSubclassOf<UGameplayAbility> &Ability : PassivesToGrant)
    {
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1.0f);
        GiveAbilityAndActivateOnce(AbilitySpec);
    }
}

void UINFAbilitySystemComponent::InitializeDefaultAttributes(const TSubclassOf<UGameplayEffect> &AttributeEffect)
{
    checkf(AttributeEffect, TEXT("No valid default attributes for this characer %s"), *GetAvatarActor()->GetName());

    const FGameplayEffectContextHandle ContextHandle = MakeEffectContext();
    const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(AttributeEffect, 1.0f, ContextHandle);
    ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void UINFAbilitySystemComponent::AbilityInputPressed(FGameplayTag InputTag)
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

void UINFAbilitySystemComponent::AbilityInputReleased(FGameplayTag InputTag)
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

void UINFAbilitySystemComponent::SetDynamicProjectile(const FGameplayTag &ProjectileTag)
{
    if (!ProjectileTag.IsValid())
        return;

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
        if (UProjectileAbility *ProjectileAbility = Cast<UProjectileAbility>(Spec.Ability))
        {
            ProjectileAbility->ProjectileToSpawnTag = ProjectileTag;
            Spec.DynamicAbilityTags.AddTag(ProjectileAbility->InputTag);

            ActiveProjectileAbility = GiveAbility(Spec);
        }
    }
}

void UINFAbilitySystemComponent::AddEquipmentEffects(FINFEquipmentEntry *EquipmentEntry)
{
    FStreamableManager &Manager = UAssetManager::GetStreamableManager();
    TWeakObjectPtr<UINFAbilitySystemComponent> WeakThis(this);

    const FGameplayEffectContextHandle ContextHandle = MakeEffectContext();

    for (const FEquipmentStatEffectGroup &StatEffect : EquipmentEntry->StatEffects)
    {
        if (IsValid(StatEffect.EffectClass.Get()))
        {
            const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(StatEffect.EffectClass.Get(), StatEffect.CurrentValue, ContextHandle);
            const FActiveGameplayEffectHandle ActiveHandle = ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

            EquipmentEntry->GrantedHandles.AddEffectHandle(ActiveHandle);
        }
        else
        {
            Manager.RequestAsyncLoad(StatEffect.EffectClass.ToSoftObjectPath(),
                                     [WeakThis, StatEffect, ContextHandle, EquipmentEntry]
                                     {
                                         const FGameplayEffectSpecHandle SpecHandle = WeakThis->MakeOutgoingSpec(StatEffect.EffectClass.Get(), StatEffect.CurrentValue, ContextHandle);
                                         const FActiveGameplayEffectHandle ActiveHandle = WeakThis->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

                                         EquipmentEntry->GrantedHandles.AddEffectHandle(ActiveHandle);
                                     });
        }
    }
}

void UINFAbilitySystemComponent::RemoveEquipmentEffects(FINFEquipmentEntry *EquipmentEntry)
{
    for (auto HandleIt = EquipmentEntry->GrantedHandles.ActiveEffects.CreateIterator(); HandleIt; ++HandleIt)
    {
        RemoveActiveGameplayEffect(*HandleIt);
        HandleIt.RemoveCurrent();
    }
}

void UINFAbilitySystemComponent::ServerSetDynamicProjectile_Implementation(const FGameplayTag &ProjectileTag)
{
    SetDynamicProjectile(ProjectileTag);
}
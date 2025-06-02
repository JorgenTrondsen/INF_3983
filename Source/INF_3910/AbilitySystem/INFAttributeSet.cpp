#include "INFAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "INFGameplayTags.h"        // Added for gameplay tags
#include "AbilitySystemComponent.h" // Added for UAbilitySystemComponent

void UINFAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UINFAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UINFAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UINFAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UINFAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
}

void UINFAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData &Data)
{
    Super::PostGameplayEffectExecute(Data);

    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
    }

    if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
    {
        SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina()));
    }

    if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
    {
        HandleIncomingDamage(Data);
    }
}

void UINFAttributeSet::HandleIncomingDamage(const FGameplayEffectModCallbackData &Data)
{
    const float LocalDamage = GetIncomingDamage();
    const float CurrentHealth = GetHealth();
    SetIncomingDamage(0.f);

    SetHealth(FMath::Clamp(CurrentHealth - LocalDamage, 0.f, GetMaxHealth()));

    if ((CurrentHealth - LocalDamage) <= 0.f)
    {
        if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AbilitySystemComponent.IsValid())
        {
            UAbilitySystemComponent *TargetASC = Data.Target.AbilityActorInfo->AbilitySystemComponent.Get();

            FGameplayTagContainer TagContainer;
            TagContainer.AddTag(INFGameplayTags::Player::State_Dead);

            TargetASC->TryActivateAbilitiesByTag(TagContainer);
        }
    }
}

void UINFAttributeSet::OnRep_Health(const FGameplayAttributeData &OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UINFAttributeSet, Health, OldHealth);
}

void UINFAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData &OldMaxHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UINFAttributeSet, MaxHealth, OldMaxHealth);
}

void UINFAttributeSet::OnRep_Stamina(const FGameplayAttributeData &OldStamina)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UINFAttributeSet, Stamina, OldStamina);
}

void UINFAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData &OldMaxStamina)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UINFAttributeSet, MaxStamina, OldMaxStamina);
}

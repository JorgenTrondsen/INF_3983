// Fill out your copyright notice in the Description page of Project Settings.

#include "INFAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

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
    SetIncomingDamage(0.f);

    SetHealth(FMath::Clamp(GetHealth() - LocalDamage, 0.f, GetMaxHealth()));
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

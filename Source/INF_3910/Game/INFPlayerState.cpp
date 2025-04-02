// Fill out your copyright notice in the Description page of Project Settings.

#include "INFPlayerState.h"
#include "INF_3910/AbilitySystem/INFAbilitySystemComponent.h"
#include "INF_3910/AbilitySystem/INFAttributeSet.h"

AINFPlayerState::AINFPlayerState()
{
    NetUpdateFrequency = 100.0f;
    MinNetUpdateFrequency = 66.0f;

    INFAbilitySystemComp = CreateDefaultSubobject<UINFAbilitySystemComponent>(TEXT("AbilitySystemComp"));
    INFAbilitySystemComp->SetIsReplicated(true);
    INFAbilitySystemComp->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    INFAttributes = CreateDefaultSubobject<UINFAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent *AINFPlayerState::GetAbilitySystemComponent() const
{
    return INFAbilitySystemComp;
}

UINFAbilitySystemComponent *AINFPlayerState::GetINFAbilitySystemComponent() const
{
    return INFAbilitySystemComp;
}

UINFAttributeSet *AINFPlayerState::GetINFAttributes() const
{
    return INFAttributes;
}
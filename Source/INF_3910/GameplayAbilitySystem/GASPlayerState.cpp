// Fill out your copyright notice in the Description page of Project Settings.


#include "GASPlayerState.h"
#include "GASAbilitySystemComponent.h"
#include "GASAttributeSet.h"


AGASPlayerState::AGASPlayerState()
{
    NetUpdateFrequency = 100.0f;
    MinNetUpdateFrequency = 66.0f;

    GASAbilitySystemComp = CreateDefaultSubobject<UGASAbilitySystemComponent>(TEXT("AbilitySystemComp"));
    GASAbilitySystemComp->SetIsReplicated(true);
    GASAbilitySystemComp->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    GASAttributes = CreateDefaultSubobject<UGASAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AGASPlayerState::GetAbilitySystemComponent() const
{
    return GASAbilitySystemComp;
}

UGASAbilitySystemComponent *AGASPlayerState::GetGASAbilitySystemComponent() const
{
    return GASAbilitySystemComp;
}

UGASAttributeSet *AGASPlayerState::GetGASAttributes() const
{
    return GASAttributes;
}
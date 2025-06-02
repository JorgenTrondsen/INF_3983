#include "INFPlayerState.h"
#include "INF_3910/AbilitySystem/INFAbilitySystemComponent.h"
#include "INF_3910/AbilitySystem/INFAttributeSet.h"
#include "Net/UnrealNetwork.h"               // Include for replication
#include "INF_3910/Character/INFCharacter.h" // Include for character access
#include "GameFramework/Pawn.h"              // Include for GetPawn
#include "TimerManager.h"                    // Include for TimerManager

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

void AINFPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(AINFPlayerState, ModelPartSelectionData, COND_None, REPNOTIFY_Always);
}

void AINFPlayerState::OnRep_ModelPartSelectionData()
{
    if (AINFCharacter *Character = GetPawn<AINFCharacter>())
    {
        Character->UpdateAppearance(ModelPartSelectionData);
    }
}
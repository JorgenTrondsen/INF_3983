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

void AINFPlayerState::ApplyAppearanceData()
{
    // Try to apply immediately first
    if (AINFCharacter *Character = GetPawn<AINFCharacter>())
    {
        Character->UpdateAppearance(ModelPartSelectionData);
        return;
    }

    // If we get here, the pawn isn't ready yet - set up delayed retry
    APawn *Pawn = GetPawn();

    // Create a timer to retry applying appearance
    FTimerHandle RetryTimerHandle;
    // Use a weak pointer to self in the lambda to prevent potential issues if the PlayerState is destroyed before the timer fires
    TWeakObjectPtr<AINFPlayerState> WeakThis(this);
    GetWorld()->GetTimerManager().SetTimer(
        RetryTimerHandle,
        FTimerDelegate::CreateLambda([WeakThis]()
                                     {
            if (!WeakThis.IsValid())
            {
                return;
            }
            AINFPlayerState* StrongThis = WeakThis.Get();
            if (AINFCharacter* Character = StrongThis->GetPawn<AINFCharacter>())
            {
                Character->UpdateAppearance(StrongThis->ModelPartSelectionData);
            } }),
        0.5f, // Second delay before retry
        false // Don't loop
    );
}

void AINFPlayerState::OnRep_ModelPartSelectionData()
{
    UE_LOG(LogTemp, Log, TEXT("OnRep_ModelPartSelectionData called for PlayerState: %s (Role: %d)"), *GetNameSafe(this), GetLocalRole());
    // This function is called on clients when the variable replicates.
    // It's also called on the listen server *acting as a client* after replication.
    // We call the helper function here to handle the update logic.
    ApplyAppearanceData();
}
#include "INFPlayerState.h"
#include "INF_3910/AbilitySystem/INFAbilitySystemComponent.h"
#include "INF_3910/AbilitySystem/INFAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "INF_3910/Character/INFCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "INF_3910/Character/Customization/SaveCustomization.h"

// Constructor initializes ability system components and loads customization data
AINFPlayerState::AINFPlayerState()
{
    NetUpdateFrequency = 100.0f;
    MinNetUpdateFrequency = 66.0f;

    INFAbilitySystemComp = CreateDefaultSubobject<UINFAbilitySystemComponent>(TEXT("AbilitySystemComp"));
    INFAbilitySystemComp->SetIsReplicated(true);
    INFAbilitySystemComp->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    INFAttributes = CreateDefaultSubobject<UINFAttributeSet>(TEXT("AttributeSet"));

    if (USaveCustomization *SaveCustomization = Cast<USaveCustomization>(UGameplayStatics::LoadGameFromSlot(TEXT("CustomizationSaveSlot"), 0)))
    {
        ModelPartSelectionData = SaveCustomization->SavedModelPartSelectionData;
    }
}

// Returns the ability system component interface
UAbilitySystemComponent *AINFPlayerState::GetAbilitySystemComponent() const
{
    return INFAbilitySystemComp;
}

// Returns the INF-specific ability system component
UINFAbilitySystemComponent *AINFPlayerState::GetINFAbilitySystemComponent() const
{
    return INFAbilitySystemComp;
}

// Returns the INF attribute set
UINFAttributeSet *AINFPlayerState::GetINFAttributes() const
{
    return INFAttributes;
}

// Sets up properties for network replication
void AINFPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(AINFPlayerState, ModelPartSelectionData, COND_None, REPNOTIFY_Always);
}

// Called when ModelPartSelectionData is replicated to update character appearance
void AINFPlayerState::OnRep_ModelPartSelectionData()
{
    if (AINFCharacter *Character = GetPawn<AINFCharacter>())
    {
        Character->UpdateAppearance(ModelPartSelectionData);
    }
}
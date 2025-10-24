#include "NPCharacter.h"
#include "INF_3910/AbilitySystem/INFAbilitySystemComponent.h"
#include "INF_3910/AbilitySystem/INFAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

// Constructor that initializes NPC character settings
ANPCharacter::ANPCharacter()
{
    // NPCs will own their own ability system component
    INFAbilitySystemComp = CreateDefaultSubobject<UINFAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    INFAbilitySystemComp->SetIsReplicated(true);
    INFAbilitySystemComp->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    INFAttributes = CreateDefaultSubobject<UINFAttributeSet>(TEXT("AttributeSet"));

    // Configure NPC-specific settings
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
    GetCharacterMovement()->MaxWalkSpeed = 400.f;
}

// Called when the game starts or when spawned
void ANPCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Initialize ability system on begin play for NPCs
    if (HasAuthority())
    {
        InitAbilityActorInfo();
    }
    else
    {
        // On clients, apply appearance immediately if data is already set
        if (!NPCAppearanceData.Race.IsEmpty())
        {
            UpdateAppearance(NPCAppearanceData);
        }
    }
}

// Called when NPC is possessed by an AI controller
void ANPCharacter::PossessedBy(AController *NewController)
{
    Super::PossessedBy(NewController);

    // Initialize ability system when possessed (server only)
    if (HasAuthority())
    {
        InitAbilityActorInfo();
    }
}

// Initializes ability system component and attributes for NPCs
void ANPCharacter::InitAbilityActorInfo()
{
    if (IsValid(INFAbilitySystemComp))
    {
        // For NPCs, the ASC is owned by the character itself, not player state
        INFAbilitySystemComp->InitAbilityActorInfo(this, this);
        BindCallbacksToDependencies();

        if (HasAuthority())
        {
            InitClassDefaults();
        }

        // Apply NPC appearance after ability system is initialized
        UpdateAppearance(NPCAppearanceData);
    }
}

// Called when NPCAppearanceData replicates to clients
void ANPCharacter::OnRep_NPCAppearanceData()
{
    // Update appearance on clients when the data replicates
    UpdateAppearance(NPCAppearanceData);
}

// Registers properties for network replication
void ANPCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ANPCharacter, NPCAppearanceData);
}
#include "NPCharacter.h"
#include "INF_3910/AbilitySystem/INFAbilitySystemComponent.h"
#include "INF_3910/AbilitySystem/INFAttributeSet.h"
#include "INF_3910/UI/WidgetControllers/DialogueWidgetController.h"
#include "INF_3910/UI/INFUserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "AIController.h"

// Constructor that initializes NPC character settings
ANPCharacter::ANPCharacter()
{
    AIControllerClass = AAIController::StaticClass();

    // NPCs will own their own ability system component
    INFAbilitySystemComp = CreateDefaultSubobject<UINFAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    INFAbilitySystemComp->SetIsReplicated(true);
    INFAbilitySystemComp->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    INFAttributes = CreateDefaultSubobject<UINFAttributeSet>(TEXT("AttributeSet"));

    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
    // Enable visibility collision for interaction system
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

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

UDialogueWidgetController *ANPCharacter::GetDialogueWidgetController(APlayerController *PlayerController)
{
    if (!IsValid(DialogueWidgetController))
    {
        DialogueWidgetController = NewObject<UDialogueWidgetController>(PlayerController, DialogueWidgetControllerClass);
    }

    return DialogueWidgetController;
}

void ANPCharacter::CreateDialogueWidget(APlayerController *PlayerController, UUserWidget *Widget)
{
    TObjectPtr<UINFUserWidget> DialogueWidget = Cast<UINFUserWidget>(Widget);
    UDialogueWidgetController *WidgetController = GetDialogueWidgetController(PlayerController);
    if (WidgetController)
    {
        WidgetController->NPCName = this->DisplayName;
        DialogueWidget->SetWidgetController(WidgetController);
        WidgetController->BroadcastInitialValues();
        DialogueWidget->AddToViewport();

        FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(DialogueWidget->TakeWidget());
        PlayerController->SetInputMode(InputMode);
        PlayerController->bShowMouseCursor = true;
    }
}

void ANPCharacter::FacePlayer(APlayerController *PlayerController)
{
    if (ACharacter *PlayerCharacter = PlayerController->GetCharacter())
    {
        FVector NPCEyeLocation = GetMesh()->GetSocketLocation(FName("bone_Head"));
        FVector PlayerEyeLocation = PlayerCharacter->GetMesh()->GetSocketLocation(FName("bone_Head"));
        FRotator LookAtRotation = (PlayerEyeLocation - NPCEyeLocation).Rotation();

        // Temporarily disable replication of rotation changes on server
        if (HasAuthority())
        {
            SetReplicates(false);
        }
        SetActorRotation(FRotator(0.f, LookAtRotation.Yaw, 0.f));
        PitchRotation = LookAtRotation.Pitch;
    }
}

void ANPCharacter::StopFacingPlayer()
{
    SetActorRotation(FRotator(0.f, 0.f, 0.f));
    PitchRotation = 0.f;

    // Re-enable replication of rotation changes on server
    if (HasAuthority())
    {
        SetReplicates(true);
    }
}
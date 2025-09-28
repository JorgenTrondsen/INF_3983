#include "INFPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"
#include "INFPlayerState.h"
#include "INF_3910/Input/INFEnhancedInputComponent.h"
#include "INF_3910/Inventory/InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "INF_3910/UI/WidgetControllers/InventoryWidgetController.h"
#include "INF_3910/UI/INFUserWidget.h"
#include "INF_3910/AbilitySystem/INFAbilitySystemComponent.h"
#include "INF_3910/Equipment/EquipmentManagerComponent.h"
#include "Kismet/GameplayStatics.h"

// Constructor to initialize components and replication settings
AINFPlayerController::AINFPlayerController()
{
    bReplicates = true;

    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");
    InventoryComponent->SetIsReplicated(true);

    EquipmentComponent = CreateDefaultSubobject<UEquipmentManagerComponent>("EquipmentComponent");
}

// Set up input bindings for ability system actions
void AINFPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UINFEnhancedInputComponent *INFInputComp = Cast<UINFEnhancedInputComponent>(InputComponent))
    {
        INFInputComp->BindAbilityActions(INFInputConfig, this, &ThisClass::AbilityInputPressed, &ThisClass::AbilityInputReleased);
    }
}

// Initialize player controller and create UI widgets with delay
void AINFPlayerController::BeginPlay()
{
    Super::BeginPlay();

    BindCallbacksToDependencies();
}

// Configure network replication properties
void AINFPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AINFPlayerController, InventoryComponent);
}

// Handle ability input press events
void AINFPlayerController::AbilityInputPressed(FGameplayTag InputTag)
{
    if (IsValid(GetINFAbilitySystemComponent()))
    {
        INFAbilitySystemComp->AbilityInputPressed(InputTag);
    }
}

// Handle ability input release events
void AINFPlayerController::AbilityInputReleased(FGameplayTag InputTag)
{
    if (IsValid(GetINFAbilitySystemComponent()))
    {
        INFAbilitySystemComp->AbilityInputReleased(InputTag);
    }
}

// Get the ability system component from player state
UINFAbilitySystemComponent *AINFPlayerController::GetINFAbilitySystemComponent()
{
    if (!IsValid(INFAbilitySystemComp))
    {
        if (const AINFPlayerState *INFPlayerState = GetPlayerState<AINFPlayerState>())
        {
            INFAbilitySystemComp = INFPlayerState->GetINFAbilitySystemComponent();
        }
    }

    return INFAbilitySystemComp;
}

// Bind callbacks between inventory and equipment components
void AINFPlayerController::BindCallbacksToDependencies()
{
    if (IsValid(InventoryComponent) && IsValid(EquipmentComponent))
    {
        InventoryComponent->EquipmentItemDelegate.AddLambda(
            [this](const TSubclassOf<UEquipmentDefinition> &EquipmentDefinition, const FEquipmentEffectPackage &EffectPackage)
            {
                if (IsValid(EquipmentComponent))
                {
                    EquipmentComponent->EquipItem(EquipmentDefinition, EffectPackage);
                }
            });

        EquipmentComponent->EquipmentList.UnEquippedEntryDelegate.AddLambda(
            [this](const FINFEquipmentEntry &UnEquippedEntry)
            {
                if (IsValid(InventoryComponent))
                {
                    InventoryComponent->AddUnEquippedItemEntry(UnEquippedEntry.EntryTag, UnEquippedEntry.EffectPackage);
                }
            });
    }
}

// Blueprint implementable function to get inventory component
UInventoryComponent *AINFPlayerController::GetInventoryComponent_Implementation()
{
    return InventoryComponent;
}

// Set dynamic projectile on ability system component
void AINFPlayerController::SetDynamicProjectile_Implementation(const FGameplayTag &ProjectileTag)
{
    if (IsValid(INFAbilitySystemComp))
    {
        INFAbilitySystemComp->SetDynamicProjectile(ProjectileTag);
    }
}

// Get ability system component from possessed pawn
UAbilitySystemComponent *AINFPlayerController::GetAbilitySystemComponent() const
{
    return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
}

// Get or create inventory widget controller
UInventoryWidgetController *AINFPlayerController::GetInventoryWidgetController()
{
    if (!IsValid(InventoryWidgetController))
    {
        InventoryWidgetController = NewObject<UInventoryWidgetController>(this, InventoryWidgetControllerClass);
        InventoryWidgetController->SetOwningActor(this);
        InventoryWidgetController->BindCallbacksToDependencies();
    }

    return InventoryWidgetController;
}

// Create and setup inventory widget with controller binding
void AINFPlayerController::CreateInventoryWidget()
{
    if (UUserWidget *Widget = CreateWidget<UINFUserWidget>(this, InventoryWidgetClass))
    {
        InventoryWidget = Cast<UINFUserWidget>(Widget);
        InventoryWidget->SetWidgetController(GetInventoryWidgetController());
        InventoryWidgetController->BroadcastInitialValues();
        InventoryWidget->AddToViewport();
    }
}
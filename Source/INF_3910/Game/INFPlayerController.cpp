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
#include "INF_3910/POI/POI.h"
#include "Kismet/GameplayStatics.h"

AINFPlayerController::AINFPlayerController()
{
    bReplicates = true;

    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");
    InventoryComponent->SetIsReplicated(true);

    EquipmentComponent = CreateDefaultSubobject<UEquipmentManagerComponent>("EquipmentComponent");
}

void AINFPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UINFEnhancedInputComponent *INFInputComp = Cast<UINFEnhancedInputComponent>(InputComponent))
    {
        INFInputComp->BindAbilityActions(INFInputConfig, this, &ThisClass::AbilityInputPressed, &ThisClass::AbilityInputReleased);
    }
}

void AINFPlayerController::BeginPlay()
{
    Super::BeginPlay();

    BindCallbacksToDependencies();

    // Add delay to ensure level is fully loaded
    FTimerHandle POIWidgetTimer;
    GetWorldTimerManager().SetTimer(POIWidgetTimer, this, &AINFPlayerController::CreatePOIWidgets, 2.0f, false);
}

void AINFPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AINFPlayerController, InventoryComponent);
}

void AINFPlayerController::AbilityInputPressed(FGameplayTag InputTag)
{
    if (IsValid(GetINFAbilitySystemComponent()))
    {
        INFAbilitySystemComp->AbilityInputPressed(InputTag);
    }
}

void AINFPlayerController::AbilityInputReleased(FGameplayTag InputTag)
{
    if (IsValid(GetINFAbilitySystemComponent()))
    {
        INFAbilitySystemComp->AbilityInputReleased(InputTag);
    }
}

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

UInventoryComponent *AINFPlayerController::GetInventoryComponent_Implementation()
{
    return InventoryComponent;
}

void AINFPlayerController::SetDynamicProjectile_Implementation(const FGameplayTag &ProjectileTag)
{
    if (IsValid(INFAbilitySystemComp))
    {
        INFAbilitySystemComp->SetDynamicProjectile(ProjectileTag);
    }
}

UAbilitySystemComponent *AINFPlayerController::GetAbilitySystemComponent() const
{
    return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
}

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

void AINFPlayerController::CreatePOIWidgets()
{
    // Only create widgets if we're the local player controller
    if (!IsLocalPlayerController())
        return;

    // Create POI Status Widget
    if (POIStatusWidgetClass && !POIStatusWidget)
    {
        POIStatusWidget = CreateWidget<UPOIStatusWidget>(this, POIStatusWidgetClass);
        if (POIStatusWidget)
        {
            POIStatusWidget->AddToViewport(1); // Z-order 1 (on top)
            FindAndConnectPOI();
            UE_LOG(LogTemp, Log, TEXT("POI Status Widget created for player: %s"), *GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to create POI Status Widget"));
        }
    }

    // Create Player Score Widget
    if (PlayerScoreWidgetClass && !PlayerScoreWidget)
    {
        PlayerScoreWidget = CreateWidget<UPlayerScoreWidget>(this, PlayerScoreWidgetClass);
        if (PlayerScoreWidget)
        {
            PlayerScoreWidget->AddToViewport(0); // Z-order 0 (behind POI widget)
            UE_LOG(LogTemp, Log, TEXT("Player Score Widget created for player: %s"), *GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to create Player Score Widget"));
        }
    }
}

void AINFPlayerController::FindAndConnectPOI()
{
    if (!POIStatusWidget)
        return;

    // Find POI in the world
    TArray<AActor*> FoundPOIs;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APOI::StaticClass(), FoundPOIs);
    
    if (FoundPOIs.Num() > 0)
    {
        APOI* GamePOI = Cast<APOI>(FoundPOIs[0]);
        if (GamePOI)
        {
            POIStatusWidget->SetPOI(GamePOI);
            UE_LOG(LogTemp, Log, TEXT("PlayerController connected to POI: %s"), *GamePOI->GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerController could not find POI in level"));
        
        // Retry after a delay (POI might not be spawned yet)
        FTimerHandle RetryTimer;
        GetWorldTimerManager().SetTimer(RetryTimer, this, &AINFPlayerController::FindAndConnectPOI, 2.0f, false);
    }
}
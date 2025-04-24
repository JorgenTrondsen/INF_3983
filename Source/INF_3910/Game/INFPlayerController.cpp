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
			[this] (const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition, const FEquipmentEffectPackage& EffectPackage)            {
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








/*
Gammel GASPlayerController.cpp

// INF_3910 by Jørgen Trondsen, Marcus Ryan and Adrian Moen

#include "GASPlayerController.h"
#include "../Input/GASEnhancedInputComponent.h"
#include "GASPlayerState.h"
#include "../AbilitySystem/GASAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

AGASPlayerController::AGASPlayerController()
{
    bReplicates = true;
}

void AGASPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UGASEnhancedInputComponent *GASInputComp = Cast<UGASEnhancedInputComponent>(InputComponent))
    {
        GASInputComp->BindAbilityActions(GASInputConfig, this, &ThisClass::AbilityInputPressed, &ThisClass::AbilityInputReleased);
    }
}

void AGASPlayerController::BeginPlay()
{
    Super::BeginPlay();
}

void AGASPlayerController::AbilityInputPressed(FGameplayTag InputTag)
{
    if (IsValid(GetGASAbilitySystemComponent()))
    {
        GASAbilitySystemComp->AbilityInputPressed(InputTag);
    }
}

void AGASPlayerController::AbilityInputReleased(FGameplayTag InputTag)
{
    if (IsValid(GetGASAbilitySystemComponent()))
    {
        GASAbilitySystemComp->AbilityInputReleased(InputTag);
    }
}

UGASAbilitySystemComponent *AGASPlayerController::GetGASAbilitySystemComponent()
{
    if (!IsValid(GASAbilitySystemComp))
    {
        if (const AGASPlayerState *GASPlayerState = GetPlayerState<AGASPlayerState>())
        {
            GASAbilitySystemComp = GASPlayerState->GetGASAbilitySystemComponent();
        }
    }

    return GASAbilitySystemComp;
}

*/
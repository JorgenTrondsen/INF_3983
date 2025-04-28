#pragma once

#include "CoreMinimal.h"
#include "WidgetController.h"
#include "InventoryWidgetController.generated.h"

struct FINFInventoryEntry;
class UInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryEntrySignature, const FINFInventoryEntry &, Entry);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryItemRemoved, const int64, ItemID);
/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class INF_3910_API UInventoryWidgetController : public UWidgetController
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FInventoryEntrySignature InventoryEntryDelegate;

    UPROPERTY(BlueprintAssignable)
    FOnInventoryItemRemoved OnInventoryItemRemoved;

    void SetOwningActor(AActor *InOwner);

    void BindCallbacksToDependencies();
    void BroadcastInitialValues();

private:
    UPROPERTY()
    TObjectPtr<AActor> OwningActor;

    UPROPERTY()
    TObjectPtr<UInventoryComponent> OwningInventory;
};
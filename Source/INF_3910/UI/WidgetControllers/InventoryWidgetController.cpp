#include "InventoryWidgetController.h"
#include "INF_3910/Inventory/InventoryComponent.h"
#include "INF_3910/Interfaces/InventoryInterface.h"

// Sets the owning actor for this widget controller
void UInventoryWidgetController::SetOwningActor(AActor *InOwner)
{
    OwningActor = InOwner;
}

// Binds delegates to inventory component for handling inventory changes
void UInventoryWidgetController::BindCallbacksToDependencies()
{
    OwningInventory = IInventoryInterface::Execute_GetInventoryComponent(OwningActor);

    if (IsValid(OwningInventory))
    {
        OwningInventory->InventoryList.DirtyItemDelegate.AddLambda(
            [this](const FINFInventoryEntry &DirtyItem)
            {
                InventoryEntryDelegate.Broadcast(DirtyItem);
            });

        OwningInventory->InventoryList.InventoryItemRemovedDelegate.AddLambda(
            [this](const int64 ItemID)
            {
                OnInventoryItemRemoved.Broadcast(ItemID);
            });
    }
}

// Broadcasts all existing inventory entries to initialize the UI
void UInventoryWidgetController::BroadcastInitialValues()
{
    if (IsValid(OwningInventory))
    {
        for (const FINFInventoryEntry &Entry : OwningInventory->GetInventoryEntries())
        {
            InventoryEntryDelegate.Broadcast(Entry);
        }
    }
}
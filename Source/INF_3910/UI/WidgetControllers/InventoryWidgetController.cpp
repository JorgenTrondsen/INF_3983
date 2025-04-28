#include "InventoryWidgetController.h"
#include "INF_3910/Inventory/InventoryComponent.h"
#include "INF_3910/Interfaces/InventoryInterface.h"

void UInventoryWidgetController::SetOwningActor(AActor *InOwner)
{
    OwningActor = InOwner;
}

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
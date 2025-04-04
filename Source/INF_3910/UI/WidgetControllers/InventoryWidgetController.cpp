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
                FMasterItemDefinition Item = OwningInventory->GetItemDefinitionByTag(DirtyItem.ItemTag);
                Item.ItemQuantity = DirtyItem.Quantity;

                InventoryItemDelegate.Broadcast(Item);
            });
    }
}

void UInventoryWidgetController::BroadcastInitialValues()
{
    if (IsValid(OwningInventory))
    {
        for (const FINFInventoryEntry &Entry : OwningInventory->GetInventoryEntries())
        {
            FMasterItemDefinition Item = OwningInventory->GetItemDefinitionByTag(Entry.ItemTag);
            Item.ItemQuantity = Entry.Quantity;

            InventoryItemDelegate.Broadcast(Item);
        }
    }
}
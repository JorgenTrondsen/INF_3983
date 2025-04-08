#include "InventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "INF_3910/Equipment/EquipmentDefinition.h"
#include "INF_3910/Inventory/ItemTypesToTables.h"
#include "INF_3910/Libraries/INFAbilitySystemLibrary.h"
#include "Net/UnrealNetwork.h"

void FINFInventoryList::AddItem(const FGameplayTag &ItemTag, int32 NumItems)
{
    for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
    {
        FINFInventoryEntry &Entry = *EntryIt;

        if (Entry.ItemTag.MatchesTagExact(ItemTag))
        {
            Entry.Quantity += NumItems;
            MarkItemDirty(Entry);

            if (OwnerComponent->GetOwner()->HasAuthority())
            {
                DirtyItemDelegate.Broadcast(Entry);
            }
            return;
        }
    }

    FINFInventoryEntry &NewEntry = Entries.AddDefaulted_GetRef();
    NewEntry.ItemTag = ItemTag;
    NewEntry.Quantity = NumItems;

    if (OwnerComponent->GetOwner()->HasAuthority())
    {
        DirtyItemDelegate.Broadcast(NewEntry);
    }

    MarkItemDirty(NewEntry);
}

void FINFInventoryList::RemoveItem(const FGameplayTag &ItemTag, int32 NumItems)
{
    for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
    {
        FINFInventoryEntry &Entry = *EntryIt;

        if (Entry.ItemTag.MatchesTagExact(ItemTag))
        {
            Entry.Quantity -= NumItems;

            MarkItemDirty(Entry);

            if (OwnerComponent->GetOwner()->HasAuthority())
            {
                DirtyItemDelegate.Broadcast(Entry);
            }
        }
    }
}

bool FINFInventoryList::HasEnough(const FGameplayTag &ItemTag, int32 NumItems)
{
    for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
    {
        FINFInventoryEntry &Entry = *EntryIt;

        if (Entry.ItemTag.MatchesTagExact(ItemTag))
        {
            if (Entry.Quantity >= NumItems)
            {
                return true;
            }
        }
    }

    return false;
}

void FINFInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
    // If you can figure out what to do with this go for it. I don't know what it is reliably good for.
}

void FINFInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
    for (const int32 Index : AddedIndices)
    {
        FINFInventoryEntry &Entry = Entries[Index];

        DirtyItemDelegate.Broadcast(Entry);
    }
}

void FINFInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
    for (const int32 Index : ChangedIndices)
    {
        FINFInventoryEntry &Entry = Entries[Index];

        DirtyItemDelegate.Broadcast(Entry);
    }
}

UInventoryComponent::UInventoryComponent() : InventoryList(this)
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UInventoryComponent, InventoryList);
}

void UInventoryComponent::AddItem(const FGameplayTag &ItemTag, int32 NumItems)
{
    AActor *Owner = GetOwner();
    if (!IsValid(Owner))
        return;

    if (!Owner->HasAuthority())
    {
        ServerAddItem(ItemTag, NumItems);
        return;
    }

    InventoryList.AddItem(ItemTag, NumItems);
}

void UInventoryComponent::ServerAddItem_Implementation(const FGameplayTag &ItemTag, int32 NumItems)
{
    AddItem(ItemTag, NumItems);
}

void UInventoryComponent::UseItem(const FGameplayTag &ItemTag, int32 NumItems)
{
    AActor *Owner = GetOwner();
    if (!IsValid(Owner))
        return;

    if (!Owner->HasAuthority())
    {
        ServerUseItem(ItemTag, NumItems);
        return;
    }

    if (InventoryList.HasEnough(ItemTag, NumItems))
    {
        const FMasterItemDefinition Item = GetItemDefinitionByTag(ItemTag);

        if (UAbilitySystemComponent *OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner))
        {
            if (IsValid(Item.ConsumableProps.ItemEffectClass))
            {
                const FGameplayEffectContextHandle ContextHandle = OwnerASC->MakeEffectContext();

                const FGameplayEffectSpecHandle SpecHandle = OwnerASC->MakeOutgoingSpec(Item.ConsumableProps.ItemEffectClass,
                                                                                        Item.ConsumableProps.ItemEffectLevel, ContextHandle);

                OwnerASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

                InventoryList.RemoveItem(ItemTag);

                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(TEXT("Server Item Used: %s"), *Item.ItemTag.ToString()));
            }
            if (IsValid(Item.EquipmentItemProps.EquipmentClass))
            {
                EquipmentItemDelegate.Broadcast(Item.EquipmentItemProps.EquipmentClass);
                InventoryList.RemoveItem(ItemTag);
            }
        }
    }
}

void UInventoryComponent::ServerUseItem_Implementation(const FGameplayTag &ItemTag, int32 NumItems)
{
    UseItem(ItemTag, NumItems);
}

FMasterItemDefinition UInventoryComponent::GetItemDefinitionByTag(const FGameplayTag &ItemTag) const
{
    checkf(InventoryDefinitions, TEXT("No Inventory Definitions Inside Component %s"), *GetNameSafe(this));

    for (const auto &Pair : InventoryDefinitions->TagsToTables)
    {
        if (ItemTag.MatchesTag(Pair.Key))
        {
            return *UINFAbilitySystemLibrary::GetDataTableRowByTag<FMasterItemDefinition>(Pair.Value, ItemTag);
        }
    }

    return FMasterItemDefinition();
}

TArray<FINFInventoryEntry> UInventoryComponent::GetInventoryEntries()
{
    return InventoryList.Entries;
}
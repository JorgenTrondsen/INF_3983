#include "InventoryComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"
#include "INF_3910/Equipment/EquipmentStatEffects.h"
#include "INF_3910/Equipment/EquipmentDefinition.h"
#include "INF_3910/Inventory/ItemTypesToTables.h"
#include "INF_3910/Libraries/AbilitySystemLibrary.h"
#include "Net/UnrealNetwork.h"

namespace GameplayTags::Static
{
    UE_DEFINE_GAMEPLAY_TAG_STATIC(Category_Equipment, "Item.Equipment");
}

// Adds an item to the inventory, either stacking or creating a new entry
void FINFInventoryList::AddItem(const FGameplayTag &ItemTag, int32 NumItems)
{
    if (ItemTag.MatchesTag(GameplayTags::Static::Category_Equipment))
    {
        // Cannot stack these categories, do nothing.
    }
    else
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
    }

    const FMasterItemDefinition Item = OwnerComponent->GetItemDefinitionByTag(ItemTag);

    FINFInventoryEntry &NewEntry = Entries.AddDefaulted_GetRef();
    NewEntry.ItemTag = ItemTag;
    NewEntry.ItemName = Item.ItemName;
    NewEntry.Quantity = NumItems;
    NewEntry.ItemID = GenerateID();

    if (NewEntry.ItemTag.MatchesTag(GameplayTags::Static::Category_Equipment) && IsValid(WeakStats.Get()))
    {
        UEquipmentStatEffects *StatEffects = WeakStats.Get();
        const UEquipmentDefinition *EquipmentCDO = GetDefault<UEquipmentDefinition>(Item.EquipmentItemProps.EquipmentClass);

        // RollForStats(EquipmentCDO, &NewEntry, StatEffects);
        AddAbility(EquipmentCDO, &NewEntry, StatEffects);
    }

    if (OwnerComponent->GetOwner()->HasAuthority())
    {
        DirtyItemDelegate.Broadcast(NewEntry);
    }

    MarkItemDirty(NewEntry);
}

// Rolls random stats for equipment items based on their definition
void FINFInventoryList::RollForStats(const UEquipmentDefinition *EquipmentCDO, FINFInventoryEntry *Entry, UEquipmentStatEffects *StatEffects)
{
    int32 StatRollIndex = 0;
    while (StatRollIndex < 3)
    {
        const int32 RandomIndex = FMath::RandRange(0, EquipmentCDO->PossibleStatRolls.Num() - 1);
        const FGameplayTag &RandomTag = EquipmentCDO->PossibleStatRolls.GetByIndex(RandomIndex);

        for (const auto &Pair : StatEffects->MasterStatMap)
        {
            if (RandomTag.MatchesTag(Pair.Key))
            {
                if (const FEquipmentStatEffectGroup *PossibleStat = UAbilitySystemLibrary::GetDataTableRowByTag<FEquipmentStatEffectGroup>(Pair.Value, RandomTag))
                {
                    if (FMath::FRandRange(0.f, 1.f) < PossibleStat->ProbabilityToSelect)
                    {
                        FEquipmentStatEffectGroup NewStat = *PossibleStat;

                        NewStat.CurrentValue = PossibleStat->bFractionalStat ? FMath::FRandRange(PossibleStat->MinStatLevel, PossibleStat->MaxStatLevel) : FMath::TruncToInt(FMath::FRandRange(PossibleStat->MinStatLevel, PossibleStat->MaxStatLevel));

                        Entry->EffectPackage.StatEffects.Add(NewStat);
                        ++StatRollIndex;
                        break;
                    }
                }
            }
        }
    }
}

// Adds an ability to an equipment item based on its definition
void FINFInventoryList::AddAbility(const UEquipmentDefinition *EquipmentCDO, FINFInventoryEntry *Entry, UEquipmentStatEffects *StatEffects)
{
    const FGameplayTag &AbilityTag = EquipmentCDO->AbilityTag;

    for (const auto &Pair : StatEffects->MasterStatMap)
    {
        if (AbilityTag.MatchesTag(Pair.Key))
        {
            if (const FEquipmentAbilityGroup *Ability = UAbilitySystemLibrary::GetDataTableRowByTag<FEquipmentAbilityGroup>(Pair.Value, AbilityTag))
            {
                Entry->EffectPackage.Ability = *Ability;
                break;
            }
        }
    }
}

// Adds an unequipped item with predefined effect package to the inventory
void FINFInventoryList::AddUnEquippedItem(const FGameplayTag &ItemTag, const FEquipmentEffectPackage &EffectPackage)
{
    const FMasterItemDefinition Item = OwnerComponent->GetItemDefinitionByTag(ItemTag);

    FINFInventoryEntry &NewEntry = Entries.AddDefaulted_GetRef();
    NewEntry.ItemTag = ItemTag;
    NewEntry.ItemName = Item.ItemName;
    NewEntry.Quantity = 1;
    NewEntry.ItemID = GenerateID();
    NewEntry.EffectPackage = EffectPackage;

    DirtyItemDelegate.Broadcast(NewEntry);
    MarkItemDirty(NewEntry);
}

// Removes a specified quantity of an item from the inventory
void FINFInventoryList::RemoveItem(const FINFInventoryEntry &InventoryEntry, int32 NumItems)
{
    for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
    {
        FINFInventoryEntry &Entry = *EntryIt;

        if (Entry.ItemID == InventoryEntry.ItemID)
        {
            Entry.Quantity -= NumItems;

            if (Entry.Quantity > 0)
            {
                MarkItemDirty(Entry);

                if (OwnerComponent->GetOwner()->HasAuthority())
                {
                    DirtyItemDelegate.Broadcast(Entry);
                }
            }
            else
            {
                InventoryItemRemovedDelegate.Broadcast(Entry.ItemID);
                EntryIt.RemoveCurrent();
                MarkArrayDirty();
            }
            break;
        }
    }
}

// Checks if the inventory has enough of a specific item
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

// Generates a unique ID for inventory items with random bit manipulation
uint64 FINFInventoryList::GenerateID()
{
    uint64 NewID = ++LastAssignedID;

    int32 SignatureIndex = 0;
    while (SignatureIndex < 12)
    {
        if (FMath::RandRange(0, 100) < 85)
        {
            NewID |= (uint64)1 << FMath::RandRange(0, 63);
        }
        ++SignatureIndex;
    }

    return NewID;
}

// Sets the stat effects reference for the inventory list
void FINFInventoryList::SetStats(UEquipmentStatEffects *InStats)
{
    WeakStats = InStats;
}

// Handles cleanup when inventory entries are removed during replication
void FINFInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
    for (const int32 Index : RemovedIndices)
    {
        const FINFInventoryEntry &Entry = Entries[Index];

        InventoryItemRemovedDelegate.Broadcast(Entry.ItemID);
    }
}

// Handles notification when inventory entries are added during replication
void FINFInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
    for (const int32 Index : AddedIndices)
    {
        FINFInventoryEntry &Entry = Entries[Index];

        DirtyItemDelegate.Broadcast(Entry);
    }
}

// Handles notification when inventory entries are changed during replication
void FINFInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
    for (const int32 Index : ChangedIndices)
    {
        FINFInventoryEntry &Entry = Entries[Index];

        DirtyItemDelegate.Broadcast(Entry);
    }
}

// Initializes the inventory component with default settings
UInventoryComponent::UInventoryComponent() : InventoryList(this)
{
    PrimaryComponentTick.bCanEverTick = false;
}

// Sets up network replication properties for the component
void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UInventoryComponent, InventoryList);
}

// Initializes component during gameplay start
void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    if (GetOwner()->HasAuthority())
    {
        InventoryList.SetStats(StatEffects);
    }
}

// Adds an item to the inventory, handling server authority
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

// Server RPC implementation for adding items
void UInventoryComponent::ServerAddItem_Implementation(const FGameplayTag &ItemTag, int32 NumItems)
{
    AddItem(ItemTag, NumItems);
}

// Uses an item from the inventory, applying its effects or equipping it
void UInventoryComponent::UseItem(const FINFInventoryEntry &Entry, int32 NumItems)
{
    AActor *Owner = GetOwner();
    if (!IsValid(Owner))
        return;

    if (!Owner->HasAuthority())
    {
        ServerUseItem(Entry, NumItems);
        return;
    }

    if (InventoryList.HasEnough(Entry.ItemTag, NumItems))
    {
        const FMasterItemDefinition Item = GetItemDefinitionByTag(Entry.ItemTag);

        if (UAbilitySystemComponent *OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner))
        {
            if (IsValid(Item.ConsumableProps.ItemEffectClass))
            {
                const FGameplayEffectContextHandle ContextHandle = OwnerASC->MakeEffectContext();

                const FGameplayEffectSpecHandle SpecHandle = OwnerASC->MakeOutgoingSpec(Item.ConsumableProps.ItemEffectClass,
                                                                                        Item.ConsumableProps.ItemEffectLevel, ContextHandle);

                OwnerASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

                InventoryList.RemoveItem(Entry);
            }
            if (IsValid(Item.EquipmentItemProps.EquipmentClass))
            {
                EquipmentItemDelegate.Broadcast(Item.EquipmentItemProps.EquipmentClass, Entry.EffectPackage);
                InventoryList.RemoveItem(Entry);
            }
        }
    }
}

// Server RPC implementation for using items
void UInventoryComponent::ServerUseItem_Implementation(const FINFInventoryEntry &Entry, int32 NumItems)
{
    UseItem(Entry, NumItems);
}

// Retrieves item definition data by gameplay tag
FMasterItemDefinition UInventoryComponent::GetItemDefinitionByTag(const FGameplayTag &ItemTag) const
{
    checkf(InventoryDefinitions, TEXT("No Inventory Definitions Inside Component %s"), *GetNameSafe(this));

    for (const auto &Pair : InventoryDefinitions->TagsToTables)
    {
        if (ItemTag.MatchesTag(Pair.Key))
        {
            return *UAbilitySystemLibrary::GetDataTableRowByTag<FMasterItemDefinition>(Pair.Value, ItemTag);
        }
    }

    return FMasterItemDefinition();
}

// Returns a copy of all inventory entries
TArray<FINFInventoryEntry> UInventoryComponent::GetInventoryEntries()
{
    return InventoryList.Entries;
}

// Adds an unequipped item entry to the inventory
void UInventoryComponent::AddUnEquippedItemEntry(const FGameplayTag &ItemTag, const FEquipmentEffectPackage &EffectPackage)
{
    InventoryList.AddUnEquippedItem(ItemTag, EffectPackage);
}

// Clears all items from the inventory
void UInventoryComponent::ClearAllInventoryItems()
{
    AActor *Owner = GetOwner();
    if (!IsValid(Owner))
        return;

    if (!Owner->HasAuthority())
    {
        ServerClearAllInventoryItems();
        return;
    }

    TArray<FINFInventoryEntry> CurrentEntries = GetInventoryEntries(); // Gets a copy
    for (const FINFInventoryEntry &Entry : CurrentEntries)
    {
        if (Entry.IsValid())
        {
            InventoryList.RemoveItem(Entry, Entry.Quantity);
        }
    }
}

// Validates server RPC for using items
bool UInventoryComponent::ServerUseItem_Validate(const FINFInventoryEntry &Entry, int32 NumItems)
{
    return Entry.IsValid() && InventoryList.HasEnough(Entry.ItemTag, NumItems);
}

// Server RPC implementation for clearing all inventory items
void UInventoryComponent::ServerClearAllInventoryItems_Implementation()
{
    ClearAllInventoryItems();
}
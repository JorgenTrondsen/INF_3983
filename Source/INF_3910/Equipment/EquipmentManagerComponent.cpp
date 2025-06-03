#include "INF_3910/Equipment/EquipmentManagerComponent.h"
#include "AbilitySystemGlobals.h"
#include "INF_3910/AbilitySystem/INFAbilitySystemComponent.h"
#include "INF_3910/Equipment/EquipmentDefinition.h"
#include "INF_3910/Inventory/ItemInstance.h"
#include "Net/UnrealNetwork.h"
#include "Engine/DataTable.h"
#include "INF_3910/Inventory/ItemTypes.h"

// Gets the ability system component from the owner actor
UINFAbilitySystemComponent *FINFEquipmentList::GetAbilitySystemComponent()
{
    check(OwnerComponent);
    check(OwnerComponent->GetOwner());

    return Cast<UINFAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerComponent->GetOwner()));
}

// Adds a new equipment entry to the list, handling slot conflicts and item creation
UItemInstance *FINFEquipmentList::AddEntry(const TSubclassOf<UEquipmentDefinition> &EquipmentDefinition, const FEquipmentEffectPackage &EffectPackage, UDataTable *ItemTable) // Added UDataTable* ItemTable
{
    check(EquipmentDefinition);
    check(OwnerComponent);
    check(OwnerComponent->GetOwner()->HasAuthority());
    check(ItemTable);

    const UEquipmentDefinition *EquipmentCDO = GetDefault<UEquipmentDefinition>(EquipmentDefinition);

    const FMasterItemDefinition *ItemDefRow = ItemTable->FindRow<FMasterItemDefinition>(EquipmentCDO->ItemTag.GetTagName(), TEXT("FINFEquipmentList::AddEntry Context"));

    static const FGameplayTag TwoHandTag = FGameplayTag::RequestGameplayTag(FName("Equipment.Slot.TwoHand"));
    static const FGameplayTag LeftHandTag = FGameplayTag::RequestGameplayTag(FName("Equipment.Slot.LeftHand"));
    static const FGameplayTag RightHandTag = FGameplayTag::RequestGameplayTag(FName("Equipment.Slot.RightHand"));

    bool bIsTwoHanded = EquipmentCDO->SlotTags.HasTag(TwoHandTag);
    FGameplayTag ChosenSlotTag = bIsTwoHanded ? TwoHandTag : FGameplayTag();

    if (bIsTwoHanded)
    {
        for (int32 i = Entries.Num() - 1; i >= 0; --i)
        {
            const FGameplayTag &EntrySlotTag = Entries[i].SlotTag;
            if (EntrySlotTag.MatchesTag(LeftHandTag) || EntrySlotTag.MatchesTag(RightHandTag) || EntrySlotTag.MatchesTag(TwoHandTag))
            {
                RemoveEntry(Entries[i].Instance);
            }
        }
    }
    else // For one-handed items
    {
        // Remove any two-handed weapons
        for (int32 i = Entries.Num() - 1; i >= 0; --i)
        {
            if (Entries[i].SlotTag.MatchesTag(TwoHandTag))
            {
                RemoveEntry(Entries[i].Instance);
                break;
            }
        }

        const TArray<FGameplayTag> &SlotTagArray = EquipmentCDO->SlotTags.GetGameplayTagArray();

        // Try to find an open slot first
        for (const FGameplayTag &SlotTag : SlotTagArray)
        {
            bool bSlotOccupied = false;
            for (const FINFEquipmentEntry &Entry : Entries)
            {
                if (Entry.SlotTag.MatchesTagExact(SlotTag))
                {
                    bSlotOccupied = true;
                    break;
                }
            }

            if (!bSlotOccupied)
            {
                ChosenSlotTag = SlotTag;
                break;
            }
        }

        // If no open slots, replace item in first slot
        if (!ChosenSlotTag.IsValid() && SlotTagArray.Num() > 0)
        {
            ChosenSlotTag = SlotTagArray[0];

            for (int32 i = Entries.Num() - 1; i >= 0; --i)
            {
                if (Entries[i].SlotTag.MatchesTagExact(ChosenSlotTag))
                {
                    RemoveEntry(Entries[i].Instance);
                    break;
                }
            }
        }
    }

    // Create and equip the new item
    FINFEquipmentEntry &NewEntry = Entries.AddDefaulted_GetRef();
    NewEntry.EntryTag = EquipmentCDO->ItemTag;
    NewEntry.SlotTag = ChosenSlotTag;
    NewEntry.EquipmentDefinition = EquipmentDefinition;
    NewEntry.EffectPackage = EffectPackage;
    NewEntry.Instance = NewObject<UItemInstance>(OwnerComponent->GetOwner(), ItemDefRow->InstanceType);

    if (NewEntry.HasStats())
    {
        AddEquipmentStats(&NewEntry);
    }

    if (NewEntry.HasAbility())
    {
        AddEquipmentAbility(&NewEntry);
    }

    NewEntry.Instance->SpawnItemActors(ItemDefRow->ActorsToSpawn, ChosenSlotTag);
    MarkItemDirty(NewEntry);
    EquipmentEntryDelegate.Broadcast(NewEntry);

    return NewEntry.Instance;
}

// Applies stat effects from equipped item to the ability system component
void FINFEquipmentList::AddEquipmentStats(FINFEquipmentEntry *Entry)
{
    if (UINFAbilitySystemComponent *ASC = GetAbilitySystemComponent())
    {
        ASC->AddEquipmentEffects(Entry);
    }
}

// Removes stat effects from unequipped item from the ability system component
void FINFEquipmentList::RemoveEquipmentStats(FINFEquipmentEntry *Entry)
{
    if (UINFAbilitySystemComponent *ASC = GetAbilitySystemComponent())
    {
        UnEquippedEntryDelegate.Broadcast(*Entry);
        ASC->RemoveEquipmentEffects(Entry);
    }
}

// Grants equipment abilities and tags to the ability system component
void FINFEquipmentList::AddEquipmentAbility(FINFEquipmentEntry *Entry)
{
    if (UINFAbilitySystemComponent *ASC = GetAbilitySystemComponent())
    {
        ASC->AddLooseGameplayTag(Entry->EntryTag);
        ASC->AddEquipmentAbility(Entry);
    }
}

// Removes equipment abilities and tags from the ability system component
void FINFEquipmentList::RemoveEquipmentAbility(FINFEquipmentEntry *Entry)
{
    if (UINFAbilitySystemComponent *ASC = GetAbilitySystemComponent())
    {
        ASC->RemoveLooseGameplayTag(Entry->EntryTag);
        ASC->RemoveEquipmentAbility(Entry);
    }
}

// Removes an equipment entry from the list and cleans up its effects
void FINFEquipmentList::RemoveEntry(UItemInstance *ItemInstance)
{
    check(OwnerComponent);

    for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
    {
        FINFEquipmentEntry &Entry = *EntryIt;

        if (Entry.Instance == ItemInstance)
        {
            Entry.Instance->DestroySpawnedActors();
            RemoveEquipmentStats(&Entry);
            RemoveEquipmentAbility(&Entry);
            EntryIt.RemoveCurrent();
            MarkArrayDirty();
        }
    }
}

// Called before equipment entries are removed during replication
void FINFEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
    for (const int32 Index : RemovedIndices)
    {
        FINFEquipmentEntry &Entry = Entries[Index];

        EquipmentEntryDelegate.Broadcast(Entry);
    }
}

// Called after equipment entries are added during replication
void FINFEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
    for (const int32 Index : AddedIndices)
    {
        FINFEquipmentEntry &Entry = Entries[Index];

        EquipmentEntryDelegate.Broadcast(Entry);
    }
}

// Called after equipment entries are changed during replication
void FINFEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
    for (const int32 Index : ChangedIndices)
    {
        FINFEquipmentEntry &Entry = Entries[Index];

        EquipmentEntryDelegate.Broadcast(Entry);
    }
}

// Constructor for the equipment manager component
UEquipmentManagerComponent::UEquipmentManagerComponent() : EquipmentList(this)
{
    PrimaryComponentTick.bCanEverTick = true;
    SetIsReplicatedByDefault(true);
}

// Sets up replication properties for networking
void UEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UEquipmentManagerComponent, EquipmentList);
}

// Equips an item with the specified definition and effect package
void UEquipmentManagerComponent::EquipItem(const TSubclassOf<UEquipmentDefinition> &EquipmentDefinition, const FEquipmentEffectPackage &EffectPackage)
{
    if (!GetOwner()->HasAuthority())
    {
        ServerEquipItem(EquipmentDefinition, EffectPackage);
        return;
    }

    if (UItemInstance *Result = EquipmentList.AddEntry(EquipmentDefinition, EffectPackage, this->ItemDefinitionsTable))
    {
        Result->OnEquipped();
    }
}

// Unequips the specified item instance
void UEquipmentManagerComponent::UnEquipItem(UItemInstance *ItemInstance)
{
    if (!GetOwner()->HasAuthority())
    {
        ServerUnEquipItem(ItemInstance);
        return;
    }

    ItemInstance->OnUnEquipped();
    EquipmentList.RemoveEntry(ItemInstance);
}

// Removes all equipped items from the player
void UEquipmentManagerComponent::ClearAllEquipment()
{
    if (!GetOwner()->HasAuthority())
    {
        ServerClearAllEquipment();
        return;
    }

    TArray<UItemInstance *> InstancesToUnEquip;
    for (const FINFEquipmentEntry &Entry : EquipmentList.GetEntries())
    {
        if (Entry.Instance)
        {
            InstancesToUnEquip.Add(Entry.Instance);
        }
    }

    for (UItemInstance *Instance : InstancesToUnEquip)
    {
        UnEquipItem(Instance);
    }
}

// Server RPC implementation for equipping items
void UEquipmentManagerComponent::ServerEquipItem_Implementation(TSubclassOf<UEquipmentDefinition> EquipmentDefinition, const FEquipmentEffectPackage &EffectPackage)
{
    EquipItem(EquipmentDefinition, EffectPackage);
}

// Server RPC implementation for unequipping items
void UEquipmentManagerComponent::ServerUnEquipItem_Implementation(UItemInstance *ItemInstance)
{
    UnEquipItem(ItemInstance);
}

// Server RPC implementation for clearing all equipment
void UEquipmentManagerComponent::ServerClearAllEquipment_Implementation()
{
    ClearAllEquipment();
}
#include "INF_3910/Equipment/EquipmentManagerComponent.h"
#include "AbilitySystemGlobals.h"
#include "INF_3910/AbilitySystem/INFAbilitySystemComponent.h"
#include "INF_3910/Equipment/EquipmentDefinition.h"
#include "INF_3910/Inventory/ItemInstance.h"
#include "Net/UnrealNetwork.h"
#include "Engine/DataTable.h"             // Add this include
#include "INF_3910/Inventory/ItemTypes.h" // Add this include for FMasterItemDefinition

UINFAbilitySystemComponent *FINFEquipmentList::GetAbilitySystemComponent()
{
    check(OwnerComponent);
    check(OwnerComponent->GetOwner());

    return Cast<UINFAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerComponent->GetOwner()));
}

UItemInstance *FINFEquipmentList::AddEntry(const TSubclassOf<UEquipmentDefinition> &EquipmentDefinition, const FEquipmentEffectPackage &EffectPackage, UDataTable *ItemTable) // Added UDataTable* ItemTable
{
    check(EquipmentDefinition);
    check(OwnerComponent);
    check(OwnerComponent->GetOwner()->HasAuthority());
    check(ItemTable); // Check that the passed table is valid

    const UEquipmentDefinition *EquipmentCDO = GetDefault<UEquipmentDefinition>(EquipmentDefinition);

    // Access the ItemDefinitionsTable from the passed parameter
    const FMasterItemDefinition *ItemDefRow = ItemTable->FindRow<FMasterItemDefinition>(EquipmentCDO->ItemTag.GetTagName(), TEXT("FINFEquipmentList::AddEntry Context"));

    // Cache common tags
    static const FGameplayTag TwoHandTag = FGameplayTag::RequestGameplayTag(FName("Equipment.Slot.TwoHand"));
    static const FGameplayTag LeftHandTag = FGameplayTag::RequestGameplayTag(FName("Equipment.Slot.LeftHand"));
    static const FGameplayTag RightHandTag = FGameplayTag::RequestGameplayTag(FName("Equipment.Slot.RightHand"));

    // Check if this is a two-handed weapon
    bool bIsTwoHanded = EquipmentCDO->SlotTags.HasTag(TwoHandTag);
    FGameplayTag ChosenSlotTag = bIsTwoHanded ? TwoHandTag : FGameplayTag();

    // Handle equipment conflicts based on equipment type
    if (bIsTwoHanded)
    {
        // For two-handed weapons, remove anything in left/right/two hand slots
        for (int32 i = Entries.Num() - 1; i >= 0; --i)
        {
            const FGameplayTag &EntrySlotTag = Entries[i].SlotTag;
            if (EntrySlotTag.MatchesTag(LeftHandTag) || EntrySlotTag.MatchesTag(RightHandTag) || EntrySlotTag.MatchesTag(TwoHandTag))
            {
                RemoveEntry(Entries[i].Instance);
            }
        }
    }
    else // For one-handed items:
    {
        // 1. Remove any two-handed weapons
        for (int32 i = Entries.Num() - 1; i >= 0; --i)
        {
            if (Entries[i].SlotTag.MatchesTag(TwoHandTag))
            {
                RemoveEntry(Entries[i].Instance);
                break;
            }
        }

        // 2. Find an available slot
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
    NewEntry.Instance = NewObject<UItemInstance>(OwnerComponent->GetOwner(), ItemDefRow->InstanceType); // Use InstanceType from ItemDefRow

    if (NewEntry.HasStats())
    {
        AddEquipmentStats(&NewEntry);
    }

    if (NewEntry.HasAbility())
    {
        AddEquipmentAbility(&NewEntry);
    }

    NewEntry.Instance->SpawnItemActors(ItemDefRow->ActorsToSpawn, ChosenSlotTag); // Use ActorsToSpawn from ItemDefRow
    MarkItemDirty(NewEntry);
    EquipmentEntryDelegate.Broadcast(NewEntry);

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
                                     FString::Printf(TEXT("Equipped Item: %s in slot: %s"),
                                                     *NewEntry.EntryTag.ToString(), *ChosenSlotTag.ToString()));

    return NewEntry.Instance;
}

void FINFEquipmentList::AddEquipmentStats(FINFEquipmentEntry *Entry)
{
    if (UINFAbilitySystemComponent *ASC = GetAbilitySystemComponent())
    {
        ASC->AddEquipmentEffects(Entry);
    }
}

void FINFEquipmentList::RemoveEquipmentStats(FINFEquipmentEntry *Entry)
{
    if (UINFAbilitySystemComponent *ASC = GetAbilitySystemComponent())
    {
        UnEquippedEntryDelegate.Broadcast(*Entry);
        ASC->RemoveEquipmentEffects(Entry);
    }
}

void FINFEquipmentList::AddEquipmentAbility(FINFEquipmentEntry *Entry)
{
    if (UINFAbilitySystemComponent *ASC = GetAbilitySystemComponent())
    {
        ASC->AddLooseGameplayTag(Entry->EntryTag);
        ASC->AddEquipmentAbility(Entry);
    }
}

void FINFEquipmentList::RemoveEquipmentAbility(FINFEquipmentEntry *Entry)
{
    if (UINFAbilitySystemComponent *ASC = GetAbilitySystemComponent())
    {
        ASC->RemoveLooseGameplayTag(Entry->EntryTag);
        ASC->RemoveEquipmentAbility(Entry);
    }
}

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

void FINFEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
    for (const int32 Index : RemovedIndices)
    {
        FINFEquipmentEntry &Entry = Entries[Index];

        EquipmentEntryDelegate.Broadcast(Entry);

        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
                                         FString::Printf(TEXT("UnEquipped Item: %s"), *Entry.EntryTag.ToString()));
    }
}

void FINFEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
    for (const int32 Index : AddedIndices)
    {
        FINFEquipmentEntry &Entry = Entries[Index];

        EquipmentEntryDelegate.Broadcast(Entry);

        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
                                         FString::Printf(TEXT("Equipped Item: %s"), *Entry.EntryTag.ToString()));
    }
}

void FINFEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
    for (const int32 Index : ChangedIndices)
    {
        FINFEquipmentEntry &Entry = Entries[Index];

        EquipmentEntryDelegate.Broadcast(Entry);
    }
}

UEquipmentManagerComponent::UEquipmentManagerComponent() : EquipmentList(this)
{
    PrimaryComponentTick.bCanEverTick = true;
    SetIsReplicatedByDefault(true);
}

void UEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UEquipmentManagerComponent, EquipmentList);
}

void UEquipmentManagerComponent::EquipItem(const TSubclassOf<UEquipmentDefinition> &EquipmentDefinition, const FEquipmentEffectPackage &EffectPackage)
{
    if (!GetOwner()->HasAuthority())
    {
        ServerEquipItem(EquipmentDefinition, EffectPackage);
        return;
    }

    if (UItemInstance *Result = EquipmentList.AddEntry(EquipmentDefinition, EffectPackage, this->ItemDefinitionsTable)) // Pass ItemTable
    {
        Result->OnEquipped();
    }
}

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

void UEquipmentManagerComponent::ServerEquipItem_Implementation(TSubclassOf<UEquipmentDefinition> EquipmentDefinition, const FEquipmentEffectPackage &EffectPackage)
{
    EquipItem(EquipmentDefinition, EffectPackage);
}

void UEquipmentManagerComponent::ServerUnEquipItem_Implementation(UItemInstance *ItemInstance)
{
    UnEquipItem(ItemInstance);
}

void UEquipmentManagerComponent::ServerClearAllEquipment_Implementation()
{
    ClearAllEquipment();
}
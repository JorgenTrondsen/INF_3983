#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ItemTypes.h"
#include "Components/ActorComponent.h"
#include "INF_3910/Equipment/EquipmentDefinition.h"
#include "INF_3910/Equipment/EquipmentTypes.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "InventoryComponent.generated.h"

class UInventoryComponent;
class UEquipmentStatEffects;
class UItemTypesToTables;

DECLARE_MULTICAST_DELEGATE_TwoParams(FEquipmentItemUsed, const TSubclassOf<UEquipmentDefinition>& /* Equipment Definition */, const FEquipmentEffectPackage& /* Stat Effects */);
USTRUCT(BlueprintType)
struct FINFInventoryEntry : public FFastArraySerializerItem
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FGameplayTag ItemTag = FGameplayTag();

    UPROPERTY(BlueprintReadOnly)
    FText ItemName = FText();

    UPROPERTY(BlueprintReadOnly)
    int32 Quantity = 0;

    UPROPERTY(BlueprintReadOnly)
    int64 ItemID = 0;

    UPROPERTY(BlueprintReadOnly)
    FEquipmentEffectPackage EffectPackage = FEquipmentEffectPackage();

    bool IsValid() const
    {
        return ItemID != 0;
    }
};

DECLARE_MULTICAST_DELEGATE_OneParam(FDirtyInventoryItemSignature, const FINFInventoryEntry & /* Dirty Item */);

USTRUCT()
struct FINFInventoryList : public FFastArraySerializer
{
    GENERATED_BODY()

    FINFInventoryList() : OwnerComponent(nullptr)
    {
    }

    FINFInventoryList(UInventoryComponent *InComponent) : OwnerComponent(InComponent)
    {
    }

    void AddItem(const FGameplayTag &ItemTag, int32 NumItems = 1);
    void RemoveItem(const FINFInventoryEntry &Entry, int32 NumItems = 1);
    bool HasEnough(const FGameplayTag &ItemTag, int32 NumItems = 1);
    uint64 GenerateID();
    void SetStats(UEquipmentStatEffects *InStats);
    void RollForStats(const UEquipmentDefinition *EquipmentCDO, FINFInventoryEntry *Entry, UEquipmentStatEffects *StatEffects);
    void AddAbility(const UEquipmentDefinition *EquipmentCDO, FINFInventoryEntry *Entry, UEquipmentStatEffects *StatEffects);
    void AddUnEquippedItem(const FGameplayTag& ItemTag, const FEquipmentEffectPackage& EffectPackage);

    // FFastArraySerializer Contract
    void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
    void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
    void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

    bool NetDeltaSerialize(FNetDeltaSerializeInfo &DeltaParms)
    {
        return FastArrayDeltaSerialize<FINFInventoryEntry, FINFInventoryList>(Entries, DeltaParms, *this);
    }

    FDirtyInventoryItemSignature DirtyItemDelegate;

private:
    friend UInventoryComponent;

    UPROPERTY()
    TArray<FINFInventoryEntry> Entries;

    UPROPERTY(NotReplicated)
    TObjectPtr<UInventoryComponent> OwnerComponent;

    UPROPERTY(NotReplicated)
    uint64 LastAssignedID = 0;

    UPROPERTY(NotReplicated)
    TWeakObjectPtr<UEquipmentStatEffects> WeakStats;
};

template <>
struct TStructOpsTypeTraits<FINFInventoryList> : TStructOpsTypeTraitsBase2<FINFInventoryList>
{
    enum
    {
        WithNetDeltaSerializer = true
    };
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INF_3910_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    FEquipmentItemUsed EquipmentItemDelegate;

    UPROPERTY(Replicated)
    FINFInventoryList InventoryList;

    UInventoryComponent();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable)
    void AddItem(const FGameplayTag &ItemTag, int32 NumItems = 1);

    UFUNCTION(BlueprintCallable)
    void UseItem(const FINFInventoryEntry &Entry, int32 NumItems);

    UFUNCTION(BlueprintPure)
    FMasterItemDefinition GetItemDefinitionByTag(const FGameplayTag &ItemTag) const;

    TArray<FINFInventoryEntry> GetInventoryEntries();
    void AddUnEquippedItemEntry(const FGameplayTag& ItemTag, const FEquipmentEffectPackage& EffectPackage);

private:
    UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Stat Effects")
    TObjectPtr<UEquipmentStatEffects> StatEffects;

    UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Item Definitions")
    TObjectPtr<UItemTypesToTables> InventoryDefinitions;

    UFUNCTION(Server, Reliable)
    void ServerAddItem(const FGameplayTag &ItemTag, int32 NumItems);

    UFUNCTION(Server, Reliable)
    void ServerUseItem(const FINFInventoryEntry &Entry, int32 NumItems);
};
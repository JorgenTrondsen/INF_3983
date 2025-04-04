#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ItemTypes.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "InventoryComponent.generated.h"

class UItemTypesToTables;

USTRUCT(BlueprintType)
struct FINFInventoryEntry : public FFastArraySerializerItem
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FGameplayTag ItemTag = FGameplayTag();

    UPROPERTY(BlueprintReadOnly)
    int32 Quantity = 0;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FDirtyInventoryItemSignature, const FINFInventoryEntry & /* Dirty Item */);

USTRUCT()
struct FINFInventoryList : public FFastArraySerializer
{
    GENERATED_BODY()

    FINFInventoryList() : OwnerComponent(nullptr)
    {
    }

    FINFInventoryList(UActorComponent *InComponent) : OwnerComponent(InComponent)
    {
    }

    void AddItem(const FGameplayTag &ItemTag, int32 NumItems = 1);
    void RemoveItem(const FGameplayTag &ItemTag, int32 NumItems = 1);
    bool HasEnough(const FGameplayTag &ItemTag, int32 NumItems = 1);

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
    friend class UInventoryComponent;

    UPROPERTY()
    TArray<FINFInventoryEntry> Entries;

    UPROPERTY(NotReplicated)
    TObjectPtr<UActorComponent> OwnerComponent;
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
    UPROPERTY(Replicated)
    FINFInventoryList InventoryList;

    UInventoryComponent();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable)
    void AddItem(const FGameplayTag &ItemTag, int32 NumItems = 1);

    UFUNCTION(BlueprintCallable)
    void UseItem(const FGameplayTag &ItemTag, int32 NumItems);

    UFUNCTION(BlueprintPure)
    FMasterItemDefinition GetItemDefinitionByTag(const FGameplayTag &ItemTag) const;

    TArray<FINFInventoryEntry> GetInventoryEntries();

private:
    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UItemTypesToTables> InventoryDefinitions;

    UFUNCTION(Server, Reliable)
    void ServerAddItem(const FGameplayTag &ItemTag, int32 NumItems);

    UFUNCTION(Server, Reliable)
    void ServerUseItem(const FGameplayTag &ItemTag, int32 NumItems);
};
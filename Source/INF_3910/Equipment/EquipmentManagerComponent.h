#pragma once

#include "CoreMinimal.h"
#include "EquipmentTypes.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "EquipmentManagerComponent.generated.h"

class UINFAbilitySystemComponent;
class UItemInstance;
class UEquipmentDefinition;
class UEquipmentManagerComponent;
class UDataTable; // Add this forward declaration

USTRUCT(BlueprintType)
struct FINFEquipmentEntry : public FFastArraySerializerItem
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FGameplayTag EntryTag = FGameplayTag();

    UPROPERTY(BlueprintReadOnly)
    FGameplayTag SlotTag = FGameplayTag();

    UPROPERTY(BlueprintReadOnly)
    FEquipmentEffectPackage EffectPackage = FEquipmentEffectPackage();

    UPROPERTY(NotReplicated)
    FEquipmentGrantedHandles GrantedHandles = FEquipmentGrantedHandles();

    bool HasStats() const
    {
        return !EffectPackage.StatEffects.IsEmpty();
    }
    bool HasAbility() const
    {
        return EffectPackage.Ability.AbilityTag.IsValid();
    }

private:
    friend UEquipmentManagerComponent;
    friend struct FINFEquipmentList;

    UPROPERTY()
    TSubclassOf<UEquipmentDefinition> EquipmentDefinition = nullptr;

    UPROPERTY()
    TObjectPtr<UItemInstance> Instance = nullptr;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FEquipmentEntrySignature, const FINFEquipmentEntry & /* Equipment Entry */);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUnEquippedEntry, const FINFEquipmentEntry & /* UnEquipped Entry */);

USTRUCT()
struct FINFEquipmentList : public FFastArraySerializer
{
    GENERATED_BODY()

    FINFEquipmentList() : OwnerComponent(nullptr)
    {
    }

    FINFEquipmentList(UActorComponent *InComponent) : OwnerComponent(InComponent)
    {
    }

    UINFAbilitySystemComponent *GetAbilitySystemComponent();
    void AddEquipmentStats(FINFEquipmentEntry *Entry);
    void RemoveEquipmentStats(FINFEquipmentEntry *Entry);
    void AddEquipmentAbility(FINFEquipmentEntry *Entry);
    void RemoveEquipmentAbility(FINFEquipmentEntry *Entry);
    UItemInstance *AddEntry(const TSubclassOf<UEquipmentDefinition> &EquipmentDefinition, const FEquipmentEffectPackage &EffectPackage, UDataTable *ItemTable);
    void RemoveEntry(UItemInstance *ItemInstance);

    // Add this getter
    const TArray<FINFEquipmentEntry> &GetEntries() const { return Entries; }

    // FFastArraySerializer Contract
    void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
    void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
    void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

    bool NetDeltaSerialize(FNetDeltaSerializeInfo &DeltaParms)
    {
        return FastArrayDeltaSerialize<FINFEquipmentEntry, FINFEquipmentList>(Entries, DeltaParms, *this);
    }

    FEquipmentEntrySignature EquipmentEntryDelegate;
    FOnUnEquippedEntry UnEquippedEntryDelegate;

private:
    friend UEquipmentManagerComponent; // Already a friend

    UPROPERTY()
    TArray<FINFEquipmentEntry> Entries;

    UPROPERTY()
    TObjectPtr<UActorComponent> OwnerComponent;
};

template <>
struct TStructOpsTypeTraits<FINFEquipmentList> : TStructOpsTypeTraitsBase2<FINFEquipmentList>
{
    enum
    {
        WithNetDeltaSerializer = true
    };
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INF_3910_API UEquipmentManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(Replicated)
    FINFEquipmentList EquipmentList;

    UEquipmentManagerComponent();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

    void EquipItem(const TSubclassOf<UEquipmentDefinition> &EquipmentDefinition, const FEquipmentEffectPackage &EffectPackage);
    void UnEquipItem(UItemInstance *ItemInstance);

    // Add this property to hold the DataTable for item definitions
    UPROPERTY(EditDefaultsOnly, Category = "Data")
    TObjectPtr<UDataTable> ItemDefinitionsTable;

    // Add this
    void ClearAllEquipment();

private:
    UFUNCTION(Server, Reliable)
    void ServerEquipItem(TSubclassOf<UEquipmentDefinition> EquipmentDefinition, const FEquipmentEffectPackage &EffectPackage);

    UFUNCTION(Server, Reliable)
    void ServerUnEquipItem(UItemInstance *ItemInstance);

    // Add this
    UFUNCTION(Server, Reliable)
    void ServerClearAllEquipment();
};
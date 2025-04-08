#pragma once

#include "CoreMinimal.h"
#include "EquipmentTypes.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "EquipmentManagerComponent.generated.h"

class UEquipmentInstance;
class UEquipmentDefinition;
class UEquipmentManagerComponent;

USTRUCT(BlueprintType)
struct FINFEquipmentEntry : public FFastArraySerializerItem
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FGameplayTag EntryTag = FGameplayTag();

    UPROPERTY(BlueprintReadOnly)
    FGameplayTag SlotTag = FGameplayTag();

    UPROPERTY(NotReplicated)
    FEquipmentGrantedHandles GrantedHandles = FEquipmentGrantedHandles();

private:
    friend UEquipmentManagerComponent;
    friend struct FINFEquipmentList;

    UPROPERTY()
    TSubclassOf<UEquipmentDefinition> EquipmentDefinition = nullptr;

    UPROPERTY()
    TObjectPtr<UEquipmentInstance> Instance = nullptr;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FEquipmentEntrySignature, const FINFEquipmentEntry & /* Equipment Entry */);

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

    UEquipmentInstance *AddEntry(const TSubclassOf<UEquipmentDefinition> &EquipmentDefinition);
    void RemoveEntry(UEquipmentInstance *EquipmentInstance);

    // FFastArraySerializer Contract
    void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
    void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
    void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

    bool NetDeltaSerialize(FNetDeltaSerializeInfo &DeltaParms)
    {
        return FastArrayDeltaSerialize<FINFEquipmentEntry, FINFEquipmentList>(Entries, DeltaParms, *this);
    }

    FEquipmentEntrySignature EquipmentEntryDelegate;

private:
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

    void EquipItem(const TSubclassOf<UEquipmentDefinition> &EquipmentDefinition);
    void UnEquipItem(UEquipmentInstance *EquipmentInstance);

private:
    UFUNCTION(Server, Reliable)
    void ServerEquipItem(TSubclassOf<UEquipmentDefinition> EquipmentDefinition);

    UFUNCTION(Server, Reliable)
    void ServerUnEquipItem(UEquipmentInstance *EquipmentInstance);
};
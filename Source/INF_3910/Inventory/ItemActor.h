#pragma once

#include "GameFramework/Actor.h"
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "INF_3910/Interfaces/InteractInterface.h"
#include "INF_3910/Equipment/EquipmentTypes.h"
#include "ItemActor.generated.h"

struct FINFInventoryEntry;

UCLASS()
class INF_3910_API AItemActor : public AActor, public IInteractInterface
{
    GENERATED_BODY()

public:
    AItemActor();

    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> &OutLifetimeProps) const override;

    void SetParams(const FINFInventoryEntry *Entry, int32 InNumItems);
    void SetMesh(UStaticMesh *InMesh);
    void SetMesh(USkeletalMesh *InMesh);

    UFUNCTION()
    void OnRep_SkeletalMesh();

    void ApplySkeletalMesh(USkeletalMesh *InMesh);

    UPROPERTY(Replicated, BlueprintReadOnly)
    FGameplayTag ItemTag;

    UPROPERTY(ReplicatedUsing = OnRep_SkeletalMesh, BlueprintReadOnly)
    TObjectPtr<USkeletalMesh> ReplicatedSkeletalMesh;

    UPROPERTY(Replicated, BlueprintReadOnly)
    int32 NumItems = 1;

    UPROPERTY(Replicated)
    uint8 ValidationBits = 0;

    UPROPERTY(Replicated, BlueprintReadOnly)
    FEquipmentEffectPackage EffectPackage;

private:
    UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
    TObjectPtr<USceneComponent> RootScene;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<UStaticMeshComponent> StaticItemMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<USkeletalMeshComponent> SkeletalItemMesh;
};
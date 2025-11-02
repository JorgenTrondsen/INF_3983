#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "INF_3910/Inventory/ItemTypes.h"
#include "EquipmentInstance.generated.h"

class AEquipmentActor;
struct FEquipmentActorToSpawn;

/**
 *
 */
UCLASS()
class INF_3910_API UEquipmentInstance : public UObject
{
    GENERATED_BODY()

public:
    virtual void OnEquipped();
    virtual void OnUnEquipped();

    void SpawnEquipmentActors(const TArray<FEquipmentActorToSpawn> &ActorsToSpawn, const FGameplayTag &SlotTag);
    void DestroySpawnedActors();

private:
    UPROPERTY()
    TArray<AActor *> SpawnedActors;

    ACharacter *GetCharacter();
};
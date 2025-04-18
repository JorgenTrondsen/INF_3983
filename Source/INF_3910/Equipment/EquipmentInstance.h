#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "EquipmentInstance.generated.h"

struct FEquipmentActorsToSpawn;
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

    void SpawnEquipmentActors(const TArray<FEquipmentActorsToSpawn>& ActorsToSpawn, const FGameplayTag& SlotTag);
 	void DestroySpawnedActors();

private:
    UPROPERTY()
    TArray<AActor*> SpawnedActors;

    ACharacter* GetCharacter();
};
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "ItemTypes.h"
#include "ItemInstance.generated.h"

class AItemActor;
struct FItemActorToSpawn;

/**
 *
 */
UCLASS()
class INF_3910_API UItemInstance : public UObject
{
    GENERATED_BODY()

public:
    virtual void OnEquipped();
    virtual void OnUnEquipped();

    void SpawnItemActors(const TArray<FItemActorToSpawn> &ActorsToSpawn, const FGameplayTag &SlotTag);
    void DestroySpawnedActors();

private:
    UPROPERTY()
    TArray<AActor *> SpawnedActors;

    ACharacter *GetCharacter();
};
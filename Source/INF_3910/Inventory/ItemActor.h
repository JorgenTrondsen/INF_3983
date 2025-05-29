#pragma once

#include "GameFramework/Actor.h"
#include "CoreMinimal.h"
#include "GameplayTagContainer.h" // Added for FGameplayTag
#include "ItemActor.generated.h"

UCLASS()
class INF_3910_API AItemActor : public AActor
{
    GENERATED_BODY()

public:
    AItemActor();

    // The Gameplay Tag that identifies this item in the data tables/inventory system
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    FGameplayTag ItemTag;

private:
    UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
    TObjectPtr<USceneComponent> RootScene;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<UStaticMeshComponent> StaticEquipmentMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<USkeletalMeshComponent> SkeletalEquipmentMesh;
};
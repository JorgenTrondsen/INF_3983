#pragma once

#include "GameFramework/Actor.h"
#include "CoreMinimal.h"
#include "ItemActor.generated.h"

UCLASS()
class INF_3910_API AItemActor : public AActor
{
    GENERATED_BODY()

public:
    AItemActor();

private:
    UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
    TObjectPtr<USceneComponent> RootScene;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<UStaticMeshComponent> StaticEquipmentMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<USkeletalMeshComponent> SkeletalEquipmentMesh;
};
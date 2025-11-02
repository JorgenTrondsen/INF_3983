#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EquipmentActor.generated.h"

UCLASS()
class INF_3910_API AEquipmentActor : public AActor
{
    GENERATED_BODY()

public:
    AEquipmentActor();

private:
    UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
    TObjectPtr<USceneComponent> RootScene;

    UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
    TObjectPtr<UStaticMeshComponent> StaticEquipmentMesh;

    UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
    TObjectPtr<USkeletalMeshComponent> SkeletalEquipmentMesh;
};
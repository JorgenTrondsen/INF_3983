#pragma once

#include "GameFramework/Actor.h"
#include "CoreMinimal.h"
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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<UStaticMeshComponent> StaticEquipmentMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<USkeletalMeshComponent> SkeletalEquipmentMesh;
};
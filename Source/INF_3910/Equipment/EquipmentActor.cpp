#include "EquipmentActor.h"

AEquipmentActor::AEquipmentActor()
{
    PrimaryActorTick.bCanEverTick = true;

    SetReplicates(true);

    RootScene = CreateDefaultSubobject<USceneComponent>("RootScene");
    SetRootComponent(RootScene);

    StaticEquipmentMesh = CreateDefaultSubobject<UStaticMeshComponent>("EquipmentMesh");
    StaticEquipmentMesh->SetupAttachment(GetRootComponent());
    StaticEquipmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    SkeletalEquipmentMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalEquipmentMesh");
    SkeletalEquipmentMesh->SetupAttachment(GetRootComponent());
    SkeletalEquipmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
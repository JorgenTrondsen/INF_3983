#include "ItemActor.h"

AItemActor::AItemActor()
{
     PrimaryActorTick.bCanEverTick = true;

     SetReplicates(true);

     RootScene = CreateDefaultSubobject<USceneComponent>("RootScene");
     SetRootComponent(RootScene);

     StaticEquipmentMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticEquipmentMesh"); // Renamed variable
     StaticEquipmentMesh->SetupAttachment(GetRootComponent());
     StaticEquipmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

     SkeletalEquipmentMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalEquipmentMesh"); // Create Skeletal Mesh Component
     SkeletalEquipmentMesh->SetupAttachment(GetRootComponent());
     SkeletalEquipmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
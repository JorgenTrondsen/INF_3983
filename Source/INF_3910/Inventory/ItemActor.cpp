#include "ItemActor.h"

AItemActor::AItemActor()
{
     PrimaryActorTick.bCanEverTick = true;

     SetReplicates(true);

     RootScene = CreateDefaultSubobject<USceneComponent>("RootScene");
     SetRootComponent(RootScene);

     StaticEquipmentMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticEquipmentMesh");
     StaticEquipmentMesh->SetupAttachment(GetRootComponent());
     StaticEquipmentMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
     StaticEquipmentMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
     StaticEquipmentMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

     SkeletalEquipmentMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalEquipmentMesh");
     SkeletalEquipmentMesh->SetupAttachment(GetRootComponent());
     SkeletalEquipmentMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
     SkeletalEquipmentMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
     SkeletalEquipmentMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}
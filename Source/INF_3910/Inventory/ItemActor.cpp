#include "ItemActor.h"
#include "INF_3910/Inventory/InventoryComponent.h"
#include "Net/UnrealNetwork.h"

AItemActor::AItemActor()
{
     PrimaryActorTick.bCanEverTick = false;
     bReplicates = true;

     RootScene = CreateDefaultSubobject<USceneComponent>("RootScene");
     SetRootComponent(RootScene);

     // Setup collision for item pickup - allow visibility traces but ignore pawn collision
     StaticItemMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticItemMesh");
     StaticItemMesh->SetupAttachment(RootScene);
     StaticItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
     StaticItemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
     StaticItemMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
     StaticItemMesh->SetIsReplicated(true);

     // Setup collision for item pickup - allow visibility traces but ignore pawn collision
     SkeletalItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalItemMesh");
     SkeletalItemMesh->SetupAttachment(RootScene);
     SkeletalItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
     SkeletalItemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
     SkeletalItemMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AItemActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> &OutLifetimeProps) const
{
     Super::GetLifetimeReplicatedProps(OutLifetimeProps);

     DOREPLIFETIME(AItemActor, ItemTag);
     DOREPLIFETIME(AItemActor, NumItems);
     DOREPLIFETIME(AItemActor, ValidationBits);
     DOREPLIFETIME(AItemActor, ReplicatedSkeletalMesh);
}

void AItemActor::SetParams(const FINFInventoryEntry *Entry, int32 InNumItems)
{
     if (Entry)
     {
          ItemTag = Entry->ItemTag;
          EffectPackage = Entry->EffectPackage;
          NumItems = InNumItems;
     }
}

void AItemActor::SetMesh(UStaticMesh *InMesh)
{
     if (IsValid(InMesh))
     {
          StaticItemMesh->SetStaticMesh(InMesh);
          StaticItemMesh->SetVisibility(true);
          SkeletalItemMesh->SetVisibility(false);
     }
}

void AItemActor::SetMesh(USkeletalMesh *InMesh)
{
     if (IsValid(InMesh))
     {
          ReplicatedSkeletalMesh = InMesh; // Store for replication
          ApplySkeletalMesh(InMesh);
     }
}

void AItemActor::ApplySkeletalMesh(USkeletalMesh *InMesh)
{
     if (IsValid(InMesh))
     {
          SkeletalItemMesh->SetSkeletalMesh(InMesh);
          SkeletalItemMesh->SetVisibility(true);
          StaticItemMesh->SetVisibility(false);
     }
}

void AItemActor::OnRep_SkeletalMesh()
{
     ApplySkeletalMesh(ReplicatedSkeletalMesh);
}
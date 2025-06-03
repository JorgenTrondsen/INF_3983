#include "ItemInstance.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "ItemActor.h"
#include "INF_3910/Equipment/EquipmentDefinition.h"
#include "GameFramework/Character.h"

void UItemInstance::OnEquipped()
{
}

void UItemInstance::OnUnEquipped()
{
}

// Spawns item actors and attaches them to the character's mesh
void UItemInstance::SpawnItemActors(const TArray<FItemActorToSpawn> &ActorsToSpawn, const FGameplayTag &SlotTag)
{
    if (ACharacter *OwningCharacter = GetCharacter())
    {
        FStreamableManager &Manager = UAssetManager::GetStreamableManager();
        TWeakObjectPtr<UItemInstance> WeakThis(this);

        for (const FItemActorToSpawn &ActorToSpawn : ActorsToSpawn)
        {
            FName AttachSocket;
            if (const FName *MappedSocket = ActorToSpawn.SlotAttachmentMap.Find(SlotTag))
            {
                AttachSocket = *MappedSocket;
            }

            if (IsValid(ActorToSpawn.EquipmentClass.Get()))
            {
                AItemActor *NewActor = this->GetWorld()->SpawnActorDeferred<AItemActor>(
                    ActorToSpawn.EquipmentClass.Get(),
                    FTransform::Identity,
                    OwningCharacter);

                NewActor->FinishSpawning(FTransform::Identity);
                NewActor->AttachToComponent(OwningCharacter->GetMesh(),
                                            FAttachmentTransformRules::KeepRelativeTransform,
                                            AttachSocket);
                SpawnedActors.Emplace(NewActor);
            }
            else
            {
                Manager.RequestAsyncLoad(ActorToSpawn.EquipmentClass.ToSoftObjectPath(),
                                         [WeakThis, ActorToSpawn, OwningCharacter, AttachSocket]()
                                         {
                                             if (!WeakThis.IsValid())
                                                 return;

                                             AItemActor *NewActor = WeakThis->GetWorld()->SpawnActorDeferred<AItemActor>(
                                                 ActorToSpawn.EquipmentClass.Get(),
                                                 FTransform::Identity,
                                                 OwningCharacter);

                                             NewActor->FinishSpawning(FTransform::Identity);
                                             NewActor->AttachToComponent(OwningCharacter->GetMesh(),
                                                                         FAttachmentTransformRules::KeepRelativeTransform,
                                                                         AttachSocket);
                                             WeakThis->SpawnedActors.Emplace(NewActor);
                                         });
            }
        }
    }
}

// Destroys all spawned actors associated with this item instance
void UItemInstance::DestroySpawnedActors()
{
    for (AActor *Actor : SpawnedActors)
    {
        Actor->Destroy();
    }
}

// Gets the character that owns this item instance
ACharacter *UItemInstance::GetCharacter()
{
    if (const APlayerController *PlayerController = Cast<APlayerController>(GetOuter()))
    {
        return Cast<ACharacter>(PlayerController->GetPawn());
    }

    return nullptr;
}
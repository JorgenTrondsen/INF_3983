#include "EquipmentInstance.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "EquipmentActor.h"
#include "INF_3910/Equipment/EquipmentDefinition.h"
#include "GameFramework/Character.h"

void UEquipmentInstance::OnEquipped()
{
}

void UEquipmentInstance::OnUnEquipped()
{
}

// Spawns equipment actors and attaches them to the character's mesh
void UEquipmentInstance::SpawnEquipmentActors(const TArray<FEquipmentActorToSpawn> &ActorsToSpawn, const FGameplayTag &SlotTag)
{
    if (ACharacter *OwningCharacter = GetCharacter())
    {
        FStreamableManager &Manager = UAssetManager::GetStreamableManager();
        TWeakObjectPtr<UEquipmentInstance> WeakThis(this);

        for (const FEquipmentActorToSpawn &ActorToSpawn : ActorsToSpawn)
        {
            FName AttachSocket;
            if (const FName *MappedSocket = ActorToSpawn.SlotAttachmentMap.Find(SlotTag))
            {
                AttachSocket = *MappedSocket;
            }

            if (IsValid(ActorToSpawn.EquipmentClass.Get()))
            {
                AEquipmentActor *NewActor = this->GetWorld()->SpawnActorDeferred<AEquipmentActor>(
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

                                             AEquipmentActor *NewActor = WeakThis->GetWorld()->SpawnActorDeferred<AEquipmentActor>(
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
void UEquipmentInstance::DestroySpawnedActors()
{
    for (AActor *Actor : SpawnedActors)
    {
        Actor->Destroy();
    }
}

// Gets the character that owns this item instance
ACharacter *UEquipmentInstance::GetCharacter()
{
    if (const APlayerController *PlayerController = Cast<APlayerController>(GetOuter()))
    {
        return Cast<ACharacter>(PlayerController->GetPawn());
    }

    return nullptr;
}
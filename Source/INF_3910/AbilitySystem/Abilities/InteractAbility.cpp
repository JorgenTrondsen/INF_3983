#include "InteractAbility.h"
#include "INF_3910/Interfaces/InteractableInterface.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

// Constructor that initializes default interaction distance
UInteractAbility::UInteractAbility()
{
    InteractionDistance = 150.0f;
}

// Main function that handles the interaction logic with interactable actors in the world
void UInteractAbility::PerformInteraction()
{
    if (AActor *OwningActor = GetAvatarActorFromActorInfo())
    {
        if (AActor *HitActor = PerformLineTrace(OwningActor))
        {
            IInteractableInterface::Execute_OnInteract(HitActor, OwningActor);
        }
    }
}

// Performs a line trace from the player's view to detect interactable actors within range
AActor *UInteractAbility::PerformLineTrace(AActor *OwningActor)
{
    if (APlayerController *PlayerController = CurrentActorInfo->PlayerController.Get())
    {
        FVector TraceStart;
        FRotator ViewRotation;
        PlayerController->GetPlayerViewPoint(TraceStart, ViewRotation);

        FVector TraceEnd = TraceStart + (ViewRotation.Vector() * InteractionDistance);
        FHitResult HitResult;
        FCollisionQueryParams CollisionParams;
        CollisionParams.AddIgnoredActor(OwningActor);

        if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, CollisionParams))
        {
            return HitResult.GetActor();
        }
    }
    return nullptr;
}

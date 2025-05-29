#include "InteractAbility.h"
#include "INF_3910/Inventory/ItemActor.h"
#include "INF_3910/Interfaces/InventoryInterface.h"
#include "INF_3910/Inventory/InventoryComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h" // Required for camera trace
#include "GameFramework/PlayerState.h"

UInteractAbility::UInteractAbility()
{
    InteractionDistance = 150.0f; // Default interaction distance
}

void UInteractAbility::PerformInteraction()
{
    AActor *OwningActor = GetAvatarActorFromActorInfo();
    if (!OwningActor)
    {
        return;
    }

    AItemActor *HitItemActor = PerformLineTrace(OwningActor);

    if (HitItemActor)
    {
        UInventoryComponent *InventoryComponent = GetInventoryComponentFromActor(OwningActor);
        if (InventoryComponent && HitItemActor->ItemTag.IsValid())
        {
            InventoryComponent->AddItem(HitItemActor->ItemTag, 1);
            // Optionally, destroy the item actor after picking it up
            HitItemActor->Destroy();
        }
    }
}

AItemActor *UInteractAbility::PerformLineTrace(AActor *OwningActor)
{
    APlayerController *PC = Cast<APlayerController>(OwningActor->GetInstigatorController());
    if (!PC)
    {
        // Fallback to pawn if controller is not available or not a player controller
        APawn *Pawn = Cast<APawn>(OwningActor);
        if (Pawn)
            PC = Cast<APlayerController>(Pawn->GetController());
        if (!PC)
            return nullptr;
    }

    FVector TraceStart;
    FRotator ViewRotation;
    PC->GetPlayerViewPoint(TraceStart, ViewRotation);

    FVector TraceEnd = TraceStart + (ViewRotation.Vector() * InteractionDistance);
    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(OwningActor); // Ignore self

    // Perform the line trace
    GetWorld()->LineTraceSingleByChannel(
        HitResult,
        TraceStart,
        TraceEnd,
        ECC_Visibility,
        CollisionParams);

    // Draw debug line (optional)
    DrawDebugLine(GetWorld(), TraceStart, TraceEnd, HitResult.bBlockingHit ? FColor::Green : FColor::Red, false, 2.0f, 0, 1.0f);

    if (HitResult.bBlockingHit && HitResult.GetActor())
    {
        return Cast<AItemActor>(HitResult.GetActor());
    }

    return nullptr;
}

UInventoryComponent *UInteractAbility::GetInventoryComponentFromActor(AActor *Actor)
{
    if (!Actor)
        return nullptr;

    // Try to get it from PlayerController first if the actor is a pawn controlled by one
    APawn *Pawn = Cast<APawn>(Actor);
    if (Pawn)
    {
        APlayerController *PC = Cast<APlayerController>(Pawn->GetController());
        if (PC && PC->Implements<UInventoryInterface>())
        {
            return IInventoryInterface::Execute_GetInventoryComponent(PC);
        }
    }

    // Fallback: Check if the actor itself implements the interface (e.g., PlayerState)
    if (Actor->Implements<UInventoryInterface>())
    {
        return IInventoryInterface::Execute_GetInventoryComponent(Actor);
    }

    // Fallback: Check if the actor has an InventoryComponent directly
    UInventoryComponent *InvComp = Actor->FindComponentByClass<UInventoryComponent>();
    if (InvComp)
    {
        return InvComp;
    }

    // If the owning actor is a pawn, try getting it from its PlayerState
    if (Pawn && Pawn->GetPlayerState())
    {
        if (Pawn->GetPlayerState()->Implements<UInventoryInterface>())
        {
            return IInventoryInterface::Execute_GetInventoryComponent(Pawn->GetPlayerState());
        }
        InvComp = Pawn->GetPlayerState()->FindComponentByClass<UInventoryComponent>();
        if (InvComp)
        {
            return InvComp;
        }
    }

    // As a last resort, try getting it from the PlayerController associated with the AbilitySystemComponent's AvatarActor
    if (GetCurrentActorInfo() && GetCurrentActorInfo()->PlayerController.IsValid())
    {
        APlayerController *PC = GetCurrentActorInfo()->PlayerController.Get();
        if (PC && PC->Implements<UInventoryInterface>())
        {
            return IInventoryInterface::Execute_GetInventoryComponent(PC);
        }
    }

    return nullptr;
}

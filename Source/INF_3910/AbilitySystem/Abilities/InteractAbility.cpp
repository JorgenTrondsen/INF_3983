#include "InteractAbility.h"
#include "INF_3910/Inventory/ItemActor.h"
#include "INF_3910/Interfaces/InventoryInterface.h"
#include "INF_3910/Inventory/InventoryComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerState.h"

UInteractAbility::UInteractAbility()
{
    InteractionDistance = 150.0f;
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
            HitItemActor->Destroy();
        }
    }
}

AItemActor *UInteractAbility::PerformLineTrace(AActor *OwningActor)
{
    APlayerController *PC = Cast<APlayerController>(OwningActor->GetInstigatorController());

    FVector TraceStart;
    FRotator ViewRotation;
    PC->GetPlayerViewPoint(TraceStart, ViewRotation);

    FVector TraceEnd = TraceStart + (ViewRotation.Vector() * InteractionDistance);
    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(OwningActor);

    GetWorld()->LineTraceSingleByChannel(
        HitResult,
        TraceStart,
        TraceEnd,
        ECC_Visibility,
        CollisionParams);

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

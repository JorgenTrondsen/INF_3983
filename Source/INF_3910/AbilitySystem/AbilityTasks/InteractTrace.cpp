#include "INF_3910/AbilitySystem/AbilityTasks/InteractTrace.h"
#include "AbilitySystemComponent.h"
#include "INF_3910/Interfaces/InteractInterface.h"
#include "INF_3910/AbilitySystem/INFAbilitySystemComponent.h"
#include "GameFramework/Character.h"

UInteractTrace::UInteractTrace()
{
    bTickingTask = true;
}

UInteractTrace *UInteractTrace::InteractTrace(UGameplayAbility *OwningAbility)
{
    UInteractTrace *InteractTraceTask = NewAbilityTask<UInteractTrace>(OwningAbility);

    // Set InteractTraceTask from INFAbilitySystemComponent to this task
    if (UINFAbilitySystemComponent *INFASC = Cast<UINFAbilitySystemComponent>(OwningAbility->GetAbilitySystemComponentFromActorInfo()))
    {
        INFASC->InteractTraceTask = InteractTraceTask;
    }

    return InteractTraceTask;
}

void UInteractTrace::Activate()
{
    Super::Activate();

    if (UAbilitySystemComponent *ASC = AbilitySystemComponent.Get())
    {
        PlayerController = ASC->AbilityActorInfo->PlayerController.Get();
    }
}

void UInteractTrace::TickTask(float DeltaTime)
{
    if (!IsValid(PlayerController))
    {
        EndTask();
    }

    Super::TickTask(DeltaTime);

    // Perform line trace from center of screen
    FVector CameraLocation;
    FRotator CameraRotation;
    PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

    FVector TraceStart = CameraLocation;
    FVector TraceEnd = CameraLocation + (CameraRotation.Vector() * 100.0f);

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(PlayerController->GetPawn());

    GetWorld()->LineTraceSingleByChannel(InteractTraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
    ThisFrameInteractActor = InteractTraceHit.GetActor();

    if (ThisFrameInteractActor)
    {
        if (ThisFrameInteractActor != LastFrameInteractActor)
        {
            IInteractInterface::Execute_BeginInteract(ThisFrameInteractActor.GetObject(), PlayerController);
        }
    }
    if (LastFrameInteractActor)
    {
        if (LastFrameInteractActor != ThisFrameInteractActor)
        {
            IInteractInterface::Execute_EndInteract(LastFrameInteractActor.GetObject());
        }
    }

    LastFrameInteractActor = ThisFrameInteractActor;
}

void UInteractTrace::TriggerInteract()
{
    if (ThisFrameInteractActor && IsValid(PlayerController))
    {
        IInteractInterface::Execute_TriggerInteract(ThisFrameInteractActor.GetObject(), PlayerController);
    }
}

FString UInteractTrace::GetInteractAnimation()
{
    if (ThisFrameInteractActor)
    {
        static const TMap<FName, FString> ActorNameMap = {
            {FName("BP_ItemActor_Base_C"), "Loot"},
        };

        FName ActorClassName = ThisFrameInteractActor.GetObject()->GetClass()->GetFName();

        if (const FString *FoundName = ActorNameMap.Find(ActorClassName))
        {
            return *FoundName;
        }
    }

    return "";
}
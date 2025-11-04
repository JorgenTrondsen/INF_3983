#include "INFAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "INF_3910/Character/AI/NPCharacter.h"
#include "KismetAnimationLibrary.h"

// Sets up the property map to bind animation properties with the ability system for data-driven animations.
void UINFAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent *ASC)
{
    check(ASC);

    PropertyMap.Initialize(this, ASC);
}

void UINFAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    APawn *OwningPawn = TryGetPawnOwner();
    Character = Cast<ACharacter>(OwningPawn);
    if (Character)
    {
        MovementComponent = Character->GetCharacterMovement();
    }
    NPCharacter = Cast<ANPCharacter>(OwningPawn);
}

void UINFAnimInstance::SetPitchRotation()
{
    float TargetPitch = 0.0f;

    if (Character->IsPlayerControlled())
    {
        if (Character->IsLocallyControlled())
        { // Use the precise control rotation for locally controlled players
            const float ControlPitch = Character->GetControlRotation().Pitch;
            TargetPitch = FRotator::NormalizeAxis(-ControlPitch);
            PitchRotator = TargetPitch;
        }
        else
        { // Use the base aim rotation for remotely controlled players to replicate their view direction smoothly
            const float AimPitch = Character->GetBaseAimRotation().Pitch;
            TargetPitch = FRotator::NormalizeAxis(-AimPitch);
            PitchRotator = FMath::FInterpTo(PitchRotator, TargetPitch, GetWorld()->GetDeltaSeconds(), 25.0f);
        }
    }
    else // The character is an NPC
    {
        if (NPCharacter->bIsInDialogue)
        { // Use the local pitch rotation defined for the NPC during dialogue
            const float NPCPitch = NPCharacter->PitchRotation;
            TargetPitch = FRotator::NormalizeAxis(-NPCPitch);
        }
        else
        { // Use the base aim rotation for NPCs to reflect their facing direction
            const float AimPitch = Character->GetBaseAimRotation().Pitch;
            TargetPitch = FRotator::NormalizeAxis(-AimPitch);
        }
        PitchRotator = TargetPitch;
    }
}

void UINFAnimInstance::SetYawRotation(bool IsAiming)
{
    float TargetYaw = 0.0f;
    const bool InvertRotIdle = (Direction > 110.0f || Direction < -110.0f);
    const bool InvertRotCombat = (Direction > 110.0f || Direction < -80.0f);

    if ((IsAiming && InvertRotCombat) || InvertRotIdle)
    { // Invert the rotation of the mesh by adding 180 degrees when angle thresholds are met
        TargetYaw = FRotator::NormalizeAxis(Direction + 180.0f);
    }
    else
    { // Use the raw direction value without modification
        TargetYaw = Direction;
    }

    // Smoothly interpolate the YawRotator towards the target value at a constant rate.
    YawRotator = FMath::FInterpConstantTo(YawRotator, TargetYaw, GetWorld()->GetDeltaSeconds(), 700.0f);
}

void UINFAnimInstance::SetDirection(FVector Velocity)
{
    Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, Character->GetActorRotation());
}
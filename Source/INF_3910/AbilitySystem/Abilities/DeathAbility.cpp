#include "DeathAbility.h"
#include "INF_3910/Character/INFCharacter.h"
#include "INF_3910/Game/INFGameMode.h"
#include "INF_3910/AbilitySystem/INFAbilitySystemComponent.h"
#include "INF_3910/AbilitySystem/INFGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "GameplayCueManager.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "INF_3910/Character/INFCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "INF_3910/AbilitySystem/INFAttributeSet.h"

UDeathAbility::UDeathAbility()
{
}

void UDeathAbility::KillPlayer()
{
    if (const FGameplayAbilityActorInfo *ActorInfo = GetCurrentActorInfo())
    {
        if (AINFCharacter *Character = Cast<AINFCharacter>(ActorInfo->AvatarActor.Get()))
        {
            Character->SetDeadState(true);
        }
    }
}

void UDeathAbility::RespawnPlayer()
{
    if (const FGameplayAbilityActorInfo *ActorInfo = GetCurrentActorInfo())
    {
        if (AINFCharacter *Character = Cast<AINFCharacter>(ActorInfo->AvatarActor.Get()))
        {
            AActor *PlayerStart = nullptr;
            for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
            {
                PlayerStart = *It;
                // might want to add logic here to pick a specific player start
                break;
            }

            if (PlayerStart)
            {
                Character->SetActorLocationAndRotation(PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation());
            }

            if (UINFAbilitySystemComponent *ASC = Cast<UINFAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get()))
            {
                if (const UINFAttributeSet *AS = ASC->GetSet<UINFAttributeSet>())
                {
                    ASC->SetNumericAttributeBase(AS->GetHealthAttribute(), AS->GetMaxHealth());
                }
            }

            Character->SetDeadState(false);
        }
    }
}

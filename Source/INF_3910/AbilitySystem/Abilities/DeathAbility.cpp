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
#include "INF_3910/Game/INFPlayerController.h"
#include "INF_3910/Equipment/EquipmentManagerComponent.h"
#include "INF_3910/Inventory/InventoryComponent.h"

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
            TArray<AActor *> PlayerStarts;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

            AActor *PlayerStart = nullptr;
            if (PlayerStarts.Num() > 0)
            {
                PlayerStart = PlayerStarts[FMath::RandRange(0, PlayerStarts.Num() - 1)];
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

            if (AController *Controller = Character->GetController())
            {
                if (AINFPlayerController *PC = Cast<AINFPlayerController>(Controller))
                {
                    if (UEquipmentManagerComponent *EquipmentManager = PC->GetEquipmentComponent())
                    {
                        EquipmentManager->ClearAllEquipment();
                    }

                    if (UInventoryComponent *InventoryManager = PC->GetInventoryComponent())
                    {
                        InventoryManager->ClearAllInventoryItems();
                    }
                }
            }
        }
    }
}

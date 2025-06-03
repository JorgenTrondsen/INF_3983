#include "DeathAbility.h"
#include "INF_3910/Character/INFCharacter.h"
#include "INF_3910/AbilitySystem/INFAbilitySystemComponent.h"
#include "INF_3910/AbilitySystem/INFAttributeSet.h"
#include "INF_3910/Game/INFPlayerController.h"
#include "INF_3910/Equipment/EquipmentManagerComponent.h"
#include "INF_3910/Inventory/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

UDeathAbility::UDeathAbility()
{
}

// Sets the character's dead state to true when called
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

// Respawns the player at a random PlayerStart location, restores health, and clears equipment/inventory
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

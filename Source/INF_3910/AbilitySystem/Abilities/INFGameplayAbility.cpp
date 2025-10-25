#include "INFGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "INF_3910/AbilitySystem/INFAbilitySystemComponent.h"
#include "INF_3910/Game/INFPlayerState.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

// Called when this ability is granted to an actor
void UINFGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilitySpec &Spec)
{
    Super::OnGiveAbility(ActorInfo, Spec);

    OwningASC = Cast<UINFAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
    OwningPlayerState = Cast<AINFPlayerState>(ActorInfo->OwnerActor.Get());
}
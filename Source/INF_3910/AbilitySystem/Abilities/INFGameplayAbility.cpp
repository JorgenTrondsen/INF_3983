#include "INFGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "INF_3910/AbilitySystem/INFAbilitySystemComponent.h"

void UINFGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilitySpec &Spec)
{
    Super::OnGiveAbility(ActorInfo, Spec);

    OwningASC = Cast<UINFAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
}
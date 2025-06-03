#include "DamageAbility.h"
#include "AbilitySystemGlobals.h"
#include "INF_3910/AbilitySystem/AbilityTypes.h"

// Captures damage effect information from the avatar actor and target actor for damage calculations
void UDamageAbility::CaptureDamageEffectInfo(AActor *TargetActor, FDamageEffectInfo &OutInfo)
{
	if (AActor *AvatarActorFromInfo = GetAvatarActorFromActorInfo())
	{
		OutInfo.AvatarActor = AvatarActorFromInfo;
		OutInfo.BaseDamage = BaseDamage;
		OutInfo.DamageEffect = DamageEffect;
		OutInfo.SourceASC = GetAbilitySystemComponentFromActorInfo();

		if (IsValid(TargetActor))
		{
			OutInfo.TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
		}
	}
}
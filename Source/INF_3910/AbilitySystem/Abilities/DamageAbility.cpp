// Fill out your copyright notice in the Description page of Project Settings.

#include "DamageAbility.h"
#include "AbilitySystemGlobals.h"
#include "INF_3910/AbilitySystem/AbilityTypes.h"

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
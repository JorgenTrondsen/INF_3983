// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystemLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "INF_3910/AbilitySystem/AbilityTypes.h"
#include "INF_3910/Game/INFGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "INF_3910/AbilitySystem/GameplayTags.h"

// Retrieves character class default info from the game mode
UCharacterClassInfo *UAbilitySystemLibrary::GetCharacterClassDefaultInfo(const UObject *WorldContextObject)
{
	if (const AINFGameMode *INFGameMode = Cast<AINFGameMode>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return INFGameMode->GetCharacterClassDefaultInfo();
	}

	return nullptr;
}

// Retrieves projectile info from the game mode
UProjectileInfo *UAbilitySystemLibrary::GetProjectileInfo(const UObject *WorldContextObject)
{
	if (const AINFGameMode *INFGameMode = Cast<AINFGameMode>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return INFGameMode->GetProjectileInfo();
	}

	return nullptr;
}

// Applies a damage effect to the target using the ability system
void UAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectInfo &DamageEffectInfo)
{
	FGameplayEffectContextHandle ContextHandle = DamageEffectInfo.SourceASC->MakeEffectContext();
	ContextHandle.AddSourceObject(DamageEffectInfo.AvatarActor);

	const FGameplayEffectSpecHandle SpecHandle = DamageEffectInfo.SourceASC->MakeOutgoingSpec(DamageEffectInfo.DamageEffect, 0.f, ContextHandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags::Combat::Data_Damage, DamageEffectInfo.BaseDamage);

	if (IsValid(DamageEffectInfo.TargetASC))
	{
		DamageEffectInfo.TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}
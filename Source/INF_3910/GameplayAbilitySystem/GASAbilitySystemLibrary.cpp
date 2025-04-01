// Fill out your copyright notice in the Description page of Project Settings.

#include "GASAbilitySystemLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "INF_3910/GameplayAbilitySystem/AbilitySystem/AbilityTypes.h"
#include "Game/GASGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "INF_3910/GameplayAbilitySystem/AbilitySystem/GASGameplayTags.h"

UGASCharacterClassInfo *UGASAbilitySystemLibrary::GetCharacterClassDefaultInfo(const UObject *WorldContextObject)
{
    if (const AGASGameMode *GASGameMode = Cast<AGASGameMode>(UGameplayStatics::GetGameMode(WorldContextObject)))
    {
        return GASGameMode->GetCharacterClassDefaultInfo();
    }

    return nullptr;
}

UProjectileInfo* UGASAbilitySystemLibrary::GetProjectileInfo(const UObject* WorldContextObject)
{
	if (const AGASGameMode* GASGameMode = Cast<AGASGameMode>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return GASGameMode->GetProjectileInfo();
	}

	return nullptr;
}

void UGASAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectInfo& DamageEffectInfo)
 {
 	FGameplayEffectContextHandle ContextHandle = DamageEffectInfo.SourceASC->MakeEffectContext();
 	ContextHandle.AddSourceObject(DamageEffectInfo.AvatarActor);
 
 	const FGameplayEffectSpecHandle SpecHandle = DamageEffectInfo.SourceASC->MakeOutgoingSpec(DamageEffectInfo.DamageEffect, 0.f, ContextHandle);
 
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GASGameplayTags::Combat::Data_Damage, DamageEffectInfo.BaseDamage);

 	if (IsValid(DamageEffectInfo.TargetASC))
 	{
 		DamageEffectInfo.TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
 	}
 }
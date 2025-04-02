// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "INFAbilitySystemLibrary.generated.h"

struct FDamageEffectInfo;
class UProjectileInfo;
class UCharacterClassInfo;
/**
 *
 */
UCLASS()
class INF_3910_API UINFAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static UCharacterClassInfo *GetCharacterClassDefaultInfo(const UObject *WorldContextObject);

	UFUNCTION(BlueprintCallable)
	static void ApplyDamageEffect(const FDamageEffectInfo &DamageEffectInfo);

	UFUNCTION(BlueprintPure)
	static UProjectileInfo *GetProjectileInfo(const UObject *WorldContextObject);
};

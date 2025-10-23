// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "INF_3910/AbilitySystem/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AbilitySystemLibrary.generated.h"

struct FDamageEffectInfo;
class UProjectileInfo;
class UCharacterClassInfo;

// Blueprint Function Library to provide utility functions for the INF Ability System
UCLASS()
class INF_3910_API UAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static UCharacterClassInfo *GetCharacterClassDefaultInfo(const UObject *WorldContextObject);

	UFUNCTION(BlueprintPure)
	static UProjectileInfo *GetProjectileInfo(const UObject *WorldContextObject);

	UFUNCTION(BlueprintCallable)
	static void ApplyDamageEffect(const FDamageEffectInfo &DamageEffectInfo);

	template <typename T>
	static T *GetDataTableRowByTag(UDataTable *DataTable, FGameplayTag Tag);
};

template <typename T>
T *UAbilitySystemLibrary::GetDataTableRowByTag(UDataTable *DataTable, FGameplayTag Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), FString(""));
}
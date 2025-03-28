// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "INF_3910/GameplayAbilitySystem/AbilitySystem/AbilityTypes.h"
#include "Engine/DataAsset.h"
#include "ProjectileInfo.generated.h"

/**
 * 
 */
UCLASS()
class INF_3910_API UProjectileInfo : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, FProjectileParams> ProjectileInfoMap;
	
};
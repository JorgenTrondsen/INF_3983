// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GASGameMode.generated.h"


class UProjectileInfo;
class UGASCharacterClassInfo;
/**
 * 
 */
UCLASS()
class INF_3910_API AGASGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	UGASCharacterClassInfo* GetCharacterClassDefaultInfo() const;
	UProjectileInfo* GetProjectileInfo() const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Class Defaults")
	TObjectPtr<UGASCharacterClassInfo> ClassDefaults;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Projectiles")
	TObjectPtr<UProjectileInfo> ProjectileInfo;
};

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilityTypes.generated.h"

class AProjectileBase;
class UGameplayEffect;
class UAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FProjectileParams
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<AProjectileBase> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UStaticMesh> ProjectileMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float InitialSpeed = 1000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float GravityScale = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bShouldBounce = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Bounciness = 0.6f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effects")
	FGameplayTag FireSoundCueTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effects")
	FGameplayTag ReloadSoundCueTag;
};

USTRUCT(BlueprintType)
struct FDamageEffectInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> AvatarActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageEffect = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> SourceASC = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> TargetASC = nullptr;

	UPROPERTY(BlueprintReadWrite)
	float BaseDamage = 0.f;
};
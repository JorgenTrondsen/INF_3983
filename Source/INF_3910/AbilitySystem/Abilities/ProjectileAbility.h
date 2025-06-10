#pragma once

#include "CoreMinimal.h"
#include "DamageAbility.h"
#include "INF_3910/AbilitySystem/AbilityTypes.h"
#include "INF_3910/AbilitySystem/Abilities/INFGameplayAbility.h"
#include "ProjectileAbility.generated.h"

/**
 *
 */
UCLASS()
class INF_3910_API UProjectileAbility : public UDamageAbility
{
	GENERATED_BODY()

public:
	UProjectileAbility();

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilitySpec &Spec) override;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Projectile")
	FGameplayTag ProjectileToSpawnTag;

	UPROPERTY(BlueprintReadOnly, Category = "Custom Values|Projectile")
	FProjectileParams CurrentProjectileParams;

private:
	UPROPERTY()
	TObjectPtr<AActor> AvatarActorFromInfo;

	UPROPERTY()
	TObjectPtr<APawn> InstigatorPawnFromInfo;

	UFUNCTION(BlueprintCallable)
	void SpawnProjectile();

	UFUNCTION(BlueprintCallable, Category = "GameplayCue")
	void ExecuteFireSoundCue();
};
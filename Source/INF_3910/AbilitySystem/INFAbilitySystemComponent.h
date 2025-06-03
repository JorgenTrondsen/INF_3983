#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "INF_3910/Equipment/EquipmentManagerComponent.h"
#include "INFAbilitySystemComponent.generated.h"

struct FINFEquipmentEntry;
DECLARE_MULTICAST_DELEGATE(FOnAttributesGiven);
/**
 *
 */
UCLASS()
class INF_3910_API UINFAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	FOnAttributesGiven OnAttributesGiven;

	void AddCharacterAbilities(const TArray<TSubclassOf<class UGameplayAbility>> &AbilitiesToGrant);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<class UGameplayAbility>> &PassivesToGrant);
	void InitializeDefaultAttributes(const TSubclassOf<UGameplayEffect> &AttributesEffect);

	void AbilityInputPressed(FGameplayTag InputTag);
	void AbilityInputReleased(FGameplayTag InputTag);

	void SetDynamicProjectile(const FGameplayTag &ProjectileTag);

	void AddEquipmentEffects(FINFEquipmentEntry *EquipmentEntry);
	void RemoveEquipmentEffects(FINFEquipmentEntry *EquipmentEntry);
	void AddEquipmentAbility(FINFEquipmentEntry *EquipmentEntry);
	void RemoveEquipmentAbility(FINFEquipmentEntry *EquipmentEntry);

private:
	FGameplayAbilitySpecHandle ActiveProjectileAbility;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Projectile Ability")
	TSubclassOf<UGameplayAbility> DynamicProjectileAbility;

	FGameplayAbilitySpecHandle GrantEquipmentAbility(const FINFEquipmentEntry *EquipmentEntry, TSubclassOf<UGameplayAbility> AbilityClass);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool IsTagActive(FGameplayTag TagToCheck) const;

	UFUNCTION(Server, Reliable)
	void ServerSetDynamicProjectile(const FGameplayTag &ProjectileTag);
};

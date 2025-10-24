#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "INF_3910/AbilitySystem/INFAbilitySystemComponent.h"
#include "INF_3910/AbilitySystem/INFAttributeSet.h"
#include "INF_3910/Interfaces/INFAbilitySystemInterface.h"
#include "INF_3910/Character/Customization/CustomizationTypes.h"
#include "BaseCharacter.generated.h"

class UCustomizationData;
struct FModelPartSelectionData;

UCLASS(Abstract)
class INF_3910_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface, public IINFAbilitySystemInterface
{
    GENERATED_BODY()

public:
    ABaseCharacter();

    // IAbilitySystemInterface
    virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;

    // IINFAbilitySystemInterface
    virtual USceneComponent *GetDynamicSpawnPoint_Implementation() override;

    // Character customization
    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    void UpdateAppearance(const FModelPartSelectionData &ModelPartSelections);

    // Optional first-person mesh support (used by PCharacter)
    virtual USkeletalMeshComponent *GetFirstPersonMesh() const { return nullptr; }

    // Dead state management
    UFUNCTION(BlueprintCallable)
    void SetDeadState(bool bNewIsDead);

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

protected:
    // Ability System Components
    UPROPERTY(BlueprintReadOnly, Category = "Ability System")
    TObjectPtr<UINFAbilitySystemComponent> INFAbilitySystemComp;

    UPROPERTY(BlueprintReadOnly, Category = "Ability System")
    TObjectPtr<UINFAttributeSet> INFAttributes;

    // Dynamic spawn point for projectiles
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    TObjectPtr<USceneComponent> DynamicProjectileSpawnPoint;

    // Character configuration
    UPROPERTY(EditAnywhere, Category = "Custom Values|Character Info")
    FGameplayTag CharacterTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
    FName ProjectileSpawnSocketName = TEXT("projectile_spawnpoint");

    // Character customization data
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Customization")
    TObjectPtr<UCustomizationData> CustomizationData;

    // Dead state
    UPROPERTY(ReplicatedUsing = OnRep_IsDead)
    bool bIsDead;

    // Initialization methods - can be overridden by child classes
    virtual void InitAbilityActorInfo();
    virtual void InitClassDefaults();
    virtual void BindCallbacksToDependencies();

    UFUNCTION()
    virtual void OnRep_IsDead();

    // State change handlers
    virtual void ApplyDeadSettings();
    virtual void ApplyAliveSettings();

    // Blueprint events for attribute changes
    UFUNCTION(BlueprintImplementableEvent, Category = "Ability System")
    void OnHealthChanged(float CurrentHealth, float MaxHealth);

    UFUNCTION(BlueprintImplementableEvent, Category = "Ability System")
    void OnStaminaChanged(float CurrentStamina, float MaxStamina);

    UFUNCTION(BlueprintCallable, Category = "Ability System")
    void BroadcastInitialValues();
};

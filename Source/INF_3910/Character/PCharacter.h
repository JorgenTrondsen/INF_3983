#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "INF_3910/AbilitySystem/INFAbilitySystemComponent.h"
#include "INF_3910/AbilitySystem/INFAttributeSet.h"
#include "Logging/LogMacros.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ObjectPtr.h"
#include "INF_3910/Interfaces/INFAbilitySystemInterface.h"
#include "INF_3910/Character/Customization/CustomizationTypes.h"
#include "PCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UCustomizationData;
struct FModelPartSelectionData;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class INF_3910_API APCharacter : public ACharacter, public IAbilitySystemInterface, public IINFAbilitySystemInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent *CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent *FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext *DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Customization", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCustomizationData> CustomizationData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent *FP_Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent *FP_Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	FName ProjectileSpawnSocketName = TEXT("projectile_spawnpoint");

public:
	APCharacter();

	// Declare the new function
	UFUNCTION(BlueprintCallable, Category = "Character Customization")
	void UpdateAppearance(const FModelPartSelectionData &ModelPartSelections);

	virtual USceneComponent *GetDynamicSpawnPoint_Implementation() override;

	virtual void PossessedBy(AController *NewController) override;
	virtual void OnRep_PlayerState() override;

	virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthChanged(float CurrentHealth, float MaxHealth);

	UFUNCTION(BlueprintImplementableEvent)
	void OnStaminaChanged(float CurrentStamina, float MaxStamina);

protected:
	void Move(const FInputActionValue &Value);
	void Look(const FInputActionValue &Value);

	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;
	virtual void BeginPlay();

private:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> DynamicProjectileSpawnPoint;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UINFAbilitySystemComponent> INFAbilitySystemComp;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UINFAttributeSet> INFAttributes;

	UPROPERTY(EditAnywhere, Category = "Custom Values|Character Info")
	FGameplayTag CharacterTag;

	UPROPERTY(ReplicatedUsing = OnRep_IsDead)
	bool bIsDead;

	void InitAbilityActorInfo();
	void InitClassDefaults();
	void BindCallbacksToDependencies();

	UFUNCTION(BlueprintCallable)
	void BroadcastInitialValues();

public:
	FORCEINLINE class USpringArmComponent *GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent *GetFollowCamera() const { return FollowCamera; }

	UFUNCTION()
	virtual void OnRep_IsDead();
	void SetDeadState(bool bNewIsDead);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

protected:
	void ApplyDeadSettings();
	void ApplyAliveSettings();
};

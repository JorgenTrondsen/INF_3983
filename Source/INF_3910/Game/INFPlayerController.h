#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "INF_3910/Interfaces/InventoryInterface.h"
#include "INF_3910/Interfaces/INFAbilitySystemInterface.h"
#include "INFPlayerController.generated.h"

class UEquipmentManagerComponent;
class UINFAbilitySystemComponent;
class UInputConfig;
class UINFUserWidget;
class UInventoryWidgetController;
class UInventoryComponent;
class UDialogueWidgetController;
class ANPCharacter;
struct FINFInventoryEntry;
/**
 *
 */
UCLASS()
class INF_3910_API AINFPlayerController : public APlayerController, public IAbilitySystemInterface, public IINFAbilitySystemInterface, public IInventoryInterface
{
	GENERATED_BODY()

public:
	AINFPlayerController();

	virtual void SetupInputComponent() override;
	/* Implement Inventory Interface */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UInventoryComponent *GetInventoryComponent();
	virtual UInventoryComponent *GetInventoryComponent_Implementation() override;

	/* Implement INFAbilitySystemInterface */
	virtual void SetDynamicProjectile_Implementation(const FGameplayTag &ProjectileTag) override;

	virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;

	UInventoryWidgetController *GetInventoryWidgetController();
	UFUNCTION(BlueprintCallable)
	void CreateInventoryWidget();

	UFUNCTION(BlueprintPure)
	UEquipmentManagerComponent *GetEquipmentComponent() const { return EquipmentComponent; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;
	void AbilityInputPressed(FGameplayTag InputTag);
	void AbilityInputReleased(FGameplayTag InputTag);

private:
	UPROPERTY()
	TObjectPtr<UINFAbilitySystemComponent> INFAbilitySystemComp;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Input")
	TObjectPtr<UInputConfig> INFInputConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Replicated)
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UEquipmentManagerComponent> EquipmentComponent;

	UPROPERTY()
	TObjectPtr<UInventoryWidgetController> InventoryWidgetController;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Widgets")
	TSubclassOf<UInventoryWidgetController> InventoryWidgetControllerClass;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UINFUserWidget> InventoryWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Widgets")
	TSubclassOf<UINFUserWidget> InventoryWidgetClass;

	UINFAbilitySystemComponent *GetINFAbilitySystemComponent();
	void BindCallbacksToDependencies();

	void SpawnItem(const FINFInventoryEntry *Entry, int32 NumItems);
};
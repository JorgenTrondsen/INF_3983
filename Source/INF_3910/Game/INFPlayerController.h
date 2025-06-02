#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "INF_3910/Interfaces/InventoryInterface.h"
#include "INF_3910/Interfaces/INFAbilitySystemInterface.h"
#include "INF_3910/UI/POIStatusWidget.h"
#include "INF_3910/UI/PlayerScoreWidget.h"
#include "INFPlayerController.generated.h"

class UEquipmentManagerComponent;
class UINFAbilitySystemComponent;
class UINFInputConfig;
class UINFUserWidget;
class UInventoryWidgetController;
class UInventoryComponent;
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

	UFUNCTION(BlueprintCallable, Category = "UI")
	void CreatePOIWidgets();

protected:
	virtual void BeginPlay() override;
	void AbilityInputPressed(FGameplayTag InputTag);
	void AbilityInputReleased(FGameplayTag InputTag);

	// Widget classes
    UPROPERTY(EditAnywhere, Category = "UI|POI")
    TSubclassOf<class UPOIStatusWidget> POIStatusWidgetClass;

    UPROPERTY(EditAnywhere, Category = "UI|POI")
    TSubclassOf<class UPlayerScoreWidget> PlayerScoreWidgetClass;

    // Widget instances
    UPROPERTY(BlueprintReadOnly, Category = "UI|POI")
    class UPOIStatusWidget* POIStatusWidget;

    UPROPERTY(BlueprintReadOnly, Category = "UI|POI")
    class UPlayerScoreWidget* PlayerScoreWidget;

private:
	UPROPERTY()
	TObjectPtr<UINFAbilitySystemComponent> INFAbilitySystemComp;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Input")
	TObjectPtr<UINFInputConfig> INFInputConfig;

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

	// Helper function to find POI in level
    void FindAndConnectPOI();
};
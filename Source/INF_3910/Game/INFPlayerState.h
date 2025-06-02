#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "INF_3910/AbilitySystem/INFAbilitySystemComponent.h"
#include "INF_3910/AbilitySystem/INFAttributeSet.h"
#include "INF_3910/Character/Customization/CustomizationTypes.h" // Include customization types
#include "Net/UnrealNetwork.h"									 // Include for replication
#include "INFPlayerState.generated.h"

/**
 *
 */
UCLASS()
class INF_3910_API AINFPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AINFPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

	virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintPure)
	UINFAbilitySystemComponent *GetINFAbilitySystemComponent() const;

	UFUNCTION(BlueprintPure)
	UINFAttributeSet *GetINFAttributes() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_ModelPartSelectionData, Category = "Character Customization")
	FModelPartSelectionData ModelPartSelectionData;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UINFAbilitySystemComponent> INFAbilitySystemComp;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UINFAttributeSet> INFAttributes;

	UFUNCTION()
	void OnRep_ModelPartSelectionData();
};

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "INF_3910/AbilitySystem/INFAbilitySystemComponent.h"
#include "INF_3910/AbilitySystem/INFAttributeSet.h"
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

	virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintPure)
	UINFAbilitySystemComponent *GetINFAbilitySystemComponent() const;

	UFUNCTION(BlueprintPure)
	UINFAttributeSet *GetINFAttributes() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UINFAbilitySystemComponent> INFAbilitySystemComp;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UINFAttributeSet> INFAttributes;
};

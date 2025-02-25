// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GASAbilitySystemComponent.h"
#include "GASAttributeSet.h"
#include "GASPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class INF_3910_API AGASPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGASPlayerState();

	virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintPure)
	UGASAbilitySystemComponent *GetGASAbilitySystemComponent() const;

	UFUNCTION(BlueprintPure)
	UGASAttributeSet *GetGASAttributes() const;

protected:
	UPROPERTY()
	TObjectPtr<UGASAbilitySystemComponent> GASAbilitySystemComp;

	UPROPERTY()
	TObjectPtr<UGASAttributeSet> GASAttributes;
};

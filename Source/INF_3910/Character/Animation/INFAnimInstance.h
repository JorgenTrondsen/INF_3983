#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Animation/AnimInstance.h"
#include "INFAnimInstance.generated.h"

class ACharacter;
class ANPCharacter;
class UCharacterMovementComponent;

/**
 *
 */
UCLASS()
class INF_3910_API UINFAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    void InitializeWithAbilitySystem(UAbilitySystemComponent *ASC);

    virtual void NativeInitializeAnimation() override;

protected:
    UPROPERTY(BlueprintReadWrite, Category = "References")
    ACharacter *Character;

    UPROPERTY(BlueprintReadWrite, Category = "References")
    ANPCharacter *NPCharacter;

    UPROPERTY(BlueprintReadWrite, Category = "References")
    UCharacterMovementComponent *MovementComponent;

    UFUNCTION(BlueprintCallable, Category = "Animation Logic")
    void SetPitchRotation();
    UPROPERTY(BlueprintReadOnly, Category = "Animation Logic")
    float PitchRotator;

    UFUNCTION(BlueprintCallable, Category = "Animation Logic")
    void SetYawRotation(bool IsAiming = false);
    UPROPERTY(BlueprintReadOnly, Category = "Animation Logic")
    float YawRotator;

    UFUNCTION(BlueprintCallable, Category = "Animation Logic")
    void SetDirection(FVector Velocity);
    UPROPERTY(BlueprintReadOnly, Category = "Animation Logic")
    float Direction;

private:
    UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Property Map")
    FGameplayTagBlueprintPropertyMap PropertyMap;
};
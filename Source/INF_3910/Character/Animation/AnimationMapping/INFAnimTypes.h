#pragma once

#include "CoreMinimal.h"
#include "INFAnimTypes.generated.h"

// Struct to hold animation montages for a specific a type of animation
USTRUCT(BlueprintType)
struct FAnimMontages
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<TObjectPtr<UAnimMontage>> Montages;
};

// Maps animation types to their montages
USTRUCT(BlueprintType)
struct FGenderAnimations
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TMap<FString, FAnimMontages> Animations;
};

// Maps race/gender combinations to their animations
USTRUCT(BlueprintType)
struct FRaceAnimations
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Male")
    FGenderAnimations MaleAnimations;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Female")
    FGenderAnimations FemaleAnimations;
};
#pragma once

#include "CoreMinimal.h"
#include "../Enums/WeaponEnums.h"
#include "../Enums/CharacterEnums.h"
#include "AttackAnimTypes.generated.h"

// Struct to hold animation montages for a specific weapon type
USTRUCT(BlueprintType)
struct FWeaponAnimMontages
{
    GENERATED_BODY()

    // Array of attack animation montages for this weapon type
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    TArray<TObjectPtr<UAnimMontage>> AttackMontages;
};

// Maps weapon types to their animation montages for a gender
USTRUCT(BlueprintType)
struct FGenderWeaponAnimations
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    TMap<EWeaponType, FWeaponAnimMontages> WeaponAnimations;
};

// Maps genders to their weapon animations
USTRUCT(BlueprintType)
struct FRaceAnimations
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    FGenderWeaponAnimations MaleAnimations;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    FGenderWeaponAnimations FemaleAnimations;
};
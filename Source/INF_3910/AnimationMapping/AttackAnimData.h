#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AttackAnimTypes.h"
#include "../Enums/CharacterEnums.h"
#include "../Enums/WeaponEnums.h"
#include "AttackAnimData.generated.h"

/**
 * Data asset to store attack animations for all races, genders, and weapon types
 */
UCLASS(BlueprintType)
class INF_3910_API UAttackAnimData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    // Map race to race-specific animations (which contain gender and weapon type mappings)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack Animations")
    TMap<ECharacterRace, FRaceAnimations> RaceAnimations;

    // Get attack animation montages by race, gender, and weapon type
    UFUNCTION(BlueprintCallable, Category = "Attack Animations")
    TArray<UAnimMontage *> GetAttackMontages(ECharacterRace Race, ECharacterGender Gender, EWeaponType WeaponType) const;

    // Get a specific attack montage with an index
    UFUNCTION(BlueprintCallable, Category = "Attack Animations")
    UAnimMontage *GetAttackMontage(ECharacterRace Race, ECharacterGender Gender, EWeaponType WeaponType, int32 MontageIndex = 0) const;
};
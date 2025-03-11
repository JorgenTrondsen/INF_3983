#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CustomizationTypes.h"
#include "CustomizationData.generated.h"

/**
 * Data asset to store character customization options for all races and genders
 */
UCLASS(BlueprintType)
class INF_3910_API UCustomizationData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    // Map race to race-specific models (which contain both male and female)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Models")
    TMap<ECharacterRace, FRaceModels> CharacterModels;
    
    // Utility function to get model parts by race and gender
    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    FCharacterModelParts GetModelParts(ECharacterRace Race, ECharacterGender Gender) const;
};
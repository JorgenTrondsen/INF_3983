#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CustomizationTypes.h"
#include "CustomizationData.generated.h"

/**
 * Struct to hold both first and third person merged meshes
 */
USTRUCT(BlueprintType)
struct FMergedMeshes
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Character Customization")
    USkeletalMesh *ThirdPersonMesh;

    UPROPERTY(BlueprintReadOnly, Category = "Character Customization")
    USkeletalMesh *FirstPersonMesh;
};

/**
 * Data asset to store character customization options for all races and genders
 */
UCLASS(BlueprintType)
class INF_3910_API UCustomizationData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    // Map race string to race-specific models (which contain both male and female)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Models")
    TMap<FString, FRaceModels> CharacterModels;

    // Utility function to get model parts by race and gender strings
    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    FCharacterModelParts GetModelParts(const FString &Race, const FString &Gender) const;

    // Merges all skeletal meshes from the character parts into two meshes (one for third person, one for first person)
    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    FMergedMeshes MergeModelParts(const FString &Race, const FString &Gender, const TMap<FString, int32> &ModelPartSelections) const;
};
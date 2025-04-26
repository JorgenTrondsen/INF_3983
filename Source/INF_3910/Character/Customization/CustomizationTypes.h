#pragma once

#include "CoreMinimal.h"
#include "CustomizationTypes.generated.h"

USTRUCT(BlueprintType)
struct FModelPartArray
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Customization Parts")
    TArray<TObjectPtr<USkeletalMesh>> Parts;
};

// New struct to represent a single category of model-specific parts
USTRUCT(BlueprintType)
struct FModelSpecificParts
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Model-specific Parts")
    FString CategoryName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Model-specific Parts")
    TArray<FModelPartArray> CategoryParts;
};

USTRUCT(BlueprintType)
struct FCharacterModelParts
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Animation")
    TSubclassOf<UAnimInstance> AnimBlueprint;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Parts")
    TObjectPtr<USkeletalMesh> Geoset0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Parts")
    TArray<TObjectPtr<USkeletalMesh>> Torso;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Parts")
    TArray<TObjectPtr<USkeletalMesh>> Gloves;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Parts")
    TArray<TObjectPtr<USkeletalMesh>> Belt;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Parts")
    TArray<TObjectPtr<USkeletalMesh>> Trousers;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Parts")
    TArray<TObjectPtr<USkeletalMesh>> Boots;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Parts")
    TArray<TObjectPtr<USkeletalMesh>> Feet;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Parts")
    TObjectPtr<USkeletalMesh> Eyes;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Parts")
    TArray<TObjectPtr<USkeletalMesh>> Face;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Parts")
    TArray<TObjectPtr<USkeletalMesh>> Ears;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Parts")
    TArray<FModelPartArray> Hair;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Parts")
    TArray<FModelSpecificParts> SpecificParts;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics Asset")
    TObjectPtr<UPhysicsAsset> PhysicsAsset;
};

// New intermediate struct to hold gender-specific models
USTRUCT(BlueprintType)
struct FRaceModels
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Models")
    FCharacterModelParts MaleModel;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Models")
    FCharacterModelParts FemaleModel;
};

USTRUCT(BlueprintType)
struct FModelPartSelectionData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Customization")
    FString Race;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Customization")
    FString Gender;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Customization")
    TArray<FString> UniformPartNames;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Customization")
    TArray<int32> UniformIndexes;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Customization")
    TArray<FString> SpecificPartNames;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Customization")
    TArray<int32> SpecificIndexes;

    FModelPartSelectionData()
    {
        // Initialize with default uniform part names
        UniformPartNames = {"Torso", "Gloves", "Belt", "Trousers", "Boots", "Feet", "Face", "Ears", "Hair"};
        // Initialize with default values (0 for each uniform part)
        UniformIndexes.Init(0, 9);
    }

    // Add a selection (similar to TMap::Add)
    void Add(const FString &PartName, int8 SelectionIndex)
    {
        int8 ExistingIndex = UniformPartNames.Find(PartName);
        if (ExistingIndex != INDEX_NONE)
        {
            UniformIndexes[ExistingIndex] = SelectionIndex;
            return;
        }

        ExistingIndex = SpecificPartNames.Find(PartName);
        if (ExistingIndex != INDEX_NONE)
        {
            SpecificIndexes[ExistingIndex] = SelectionIndex;
        }
        else
        {
            SpecificPartNames.Add(PartName);
            SpecificIndexes.Add(SelectionIndex);
        }
    }
};
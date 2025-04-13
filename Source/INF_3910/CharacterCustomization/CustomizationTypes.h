#pragma once

#include "CoreMinimal.h"
#include "CustomizationTypes.generated.h"

// New struct to represent a single category of model-specific parts
USTRUCT(BlueprintType)
struct FModelSpecificParts
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Model-specific Parts")
    FString CategoryName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Model-specific Parts")
    TArray<TObjectPtr<USkeletalMesh>> Parts;
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
    TArray<TObjectPtr<USkeletalMesh>> Face;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Parts")
    TObjectPtr<USkeletalMesh> Eyes;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Parts")
    TArray<TObjectPtr<USkeletalMesh>> Ears;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Parts")
    TArray<TObjectPtr<USkeletalMesh>> Hair;

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
#pragma once

#include "CoreMinimal.h"
#include "CustomizationTypes.generated.h"

USTRUCT(BlueprintType)
struct FCharacterModelParts
{
    GENERATED_BODY()

    // Reference to the animation blueprint for this model
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Parts")
    TSubclassOf<UAnimInstance> AnimBlueprint;

    // The skeletal meshes for different body parts
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Parts")
    TObjectPtr<USkeletalMesh> Torso;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Parts")
    TObjectPtr<USkeletalMesh> Face;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Parts")
    TObjectPtr<USkeletalMesh> Legs;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Parts")
    TArray<TObjectPtr<USkeletalMesh>> HairOptions;
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
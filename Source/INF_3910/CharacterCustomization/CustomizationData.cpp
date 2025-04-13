#include "CustomizationData.h"
#include "SkeletalMeshMerge.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Animation/Skeleton.h"

FCharacterModelParts UCustomizationData::GetModelParts(const FString &Race, const FString &Gender) const
{
    if (const FRaceModels *RaceModels = CharacterModels.Find(Race))
    {
        if (Gender.Equals(TEXT("Male"), ESearchCase::IgnoreCase))
        {
            return RaceModels->MaleModel;
        }
        else // Assuming Female or any other gender defaults to female
        {
            return RaceModels->FemaleModel;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Race '%s' not found in character models data"), *Race);
    return FCharacterModelParts();
}

FMergedMeshes UCustomizationData::MergeModelParts(const FString &Race, const FString &Gender, const TMap<FString, int32> &ModelPartSelections) const
{
    FCharacterModelParts ModelParts;
    if (Gender.Equals(TEXT("Male")))
    {
        ModelParts = CharacterModels.Find(Race)->MaleModel;
    }
    else
    {
        ModelParts = CharacterModels.Find(Race)->FemaleModel;
    }

    // Create arrays to hold all non-null skeletal meshes
    TArray<USkeletalMesh *> MeshesToMerge;
    TArray<USkeletalMesh *> FP_MeshesToMerge;

    // Add base mesh (Geoset0)
    MeshesToMerge.Add(ModelParts.Geoset0);
    FP_MeshesToMerge.Add(ModelParts.Geoset0);

    // Add Torso mesh based on selection
    MeshesToMerge.Add(ModelParts.Torso[ModelPartSelections["Torso"]]);
    FP_MeshesToMerge.Add(ModelParts.Torso[ModelPartSelections["Torso"]]);

    // Add Gloves mesh based on selection
    MeshesToMerge.Add(ModelParts.Gloves[ModelPartSelections["Gloves"]]);
    FP_MeshesToMerge.Add(ModelParts.Gloves[ModelPartSelections["Gloves"]]);

    // Add Belt mesh based on selection
    MeshesToMerge.Add(ModelParts.Belt[ModelPartSelections["Belt"]]);
    FP_MeshesToMerge.Add(ModelParts.Belt[ModelPartSelections["Belt"]]);

    // Add Trousers mesh based on selection
    MeshesToMerge.Add(ModelParts.Trousers[ModelPartSelections["Trousers"]]);
    FP_MeshesToMerge.Add(ModelParts.Trousers[ModelPartSelections["Trousers"]]);

    // Add Boots mesh based on selection
    MeshesToMerge.Add(ModelParts.Boots[ModelPartSelections["Boots"]]);
    FP_MeshesToMerge.Add(ModelParts.Boots[ModelPartSelections["Boots"]]);

    // Add Feet mesh based on selection
    MeshesToMerge.Add(ModelParts.Feet[ModelPartSelections["Feet"]]);
    FP_MeshesToMerge.Add(ModelParts.Feet[ModelPartSelections["Feet"]]);

    // Add Face mesh based on selection
    MeshesToMerge.Add(ModelParts.Face[ModelPartSelections["Face"]]);
    FP_MeshesToMerge.Add(ModelParts.Face[ModelPartSelections["Face"]]);

    // Add Eyes mesh
    MeshesToMerge.Add(ModelParts.Eyes);
    FP_MeshesToMerge.Add(ModelParts.Eyes);

    // Add Ears mesh based on selection
    MeshesToMerge.Add(ModelParts.Ears[ModelPartSelections["Ears"]]);
    FP_MeshesToMerge.Add(ModelParts.Ears[ModelPartSelections["Ears"]]);

    // Add Hair mesh based on selection, 128 is the value for no mesh
    if (ModelPartSelections["Hair"] != 128)
    {
        MeshesToMerge.Add(ModelParts.Hair[ModelPartSelections["Hair"]]);
        FP_MeshesToMerge.Add(ModelParts.Hair[ModelPartSelections["Hair"]]);
    }

    // Process model specific parts - iterate through categories and add selected parts
    for (int32 CategoryIndex = 0; CategoryIndex < ModelParts.SpecificParts.Num(); CategoryIndex++)
    {
        const FModelSpecificParts &Category = ModelParts.SpecificParts[CategoryIndex];

        // Get the selected index for this category from the map
        int32 SelectedIndex = ModelPartSelections[Category.CategoryName];

        if (SelectedIndex == 128)
            continue; // Skip if no part is selected

        // Add the selected part
        MeshesToMerge.Add(Category.Parts[SelectedIndex]);
        FP_MeshesToMerge.Add(Category.Parts[SelectedIndex]);
    }

    // Use the first mesh as a reference for skeleton
    USkeleton *BaseSkeleton = MeshesToMerge[0]->GetSkeleton();

    // Create new skeletal meshes to hold the merged result
    USkeletalMesh *MergedMesh = NewObject<USkeletalMesh>(GetTransientPackage());
    USkeletalMesh *FP_MergedMesh = NewObject<USkeletalMesh>(GetTransientPackage());

    // Create and initialize the mesh merger
    FSkeletalMeshMerge MeshMerger(MergedMesh, MeshesToMerge, TArray<FSkelMeshMergeSectionMapping>(), 0);
    FSkeletalMeshMerge FP_MeshMerger(FP_MergedMesh, FP_MeshesToMerge, TArray<FSkelMeshMergeSectionMapping>(), 0);

    // Perform the merge
    if (!MeshMerger.DoMerge() || !FP_MeshMerger.DoMerge())
    {
        UE_LOG(LogTemp, Error, TEXT("MergeModelParts: Mesh merge operation failed"));
        return FMergedMeshes();
    }

    // Assign the skeleton & physics asset to the merged meshes
    MergedMesh->SetSkeleton(BaseSkeleton);
    MergedMesh->SetPhysicsAsset(ModelParts.PhysicsAsset);
    FP_MergedMesh->SetSkeleton(BaseSkeleton);
    FP_MergedMesh->SetPhysicsAsset(ModelParts.PhysicsAsset);

    // Set the result meshes
    FMergedMeshes Result;
    Result.ThirdPersonMesh = MergedMesh;
    Result.FirstPersonMesh = FP_MergedMesh;

    return Result;
}
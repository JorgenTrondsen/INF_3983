#include "CustomizationData.h"
#include "SkeletalMeshMerge.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Animation/Skeleton.h"

void UCustomizationData::GetModelData(const FString &Race, const FString &Gender, TMap<FString, int32> &ModelCustomizations, FModelPartSelectionData &ModelPartSelections) const
{
    const FCharacterModelParts &ModelParts = Gender.Equals(TEXT("Male"))
                                                 ? CharacterModels.Find(Race)->MaleModel
                                                 : CharacterModels.Find(Race)->FemaleModel;

    ModelPartSelections.Race = Race;
    ModelPartSelections.Gender = Gender;

    // Add the counts for Face, Ears, and Hair to ModelCustomizations
    ModelCustomizations.Add(TEXT("Face"), ModelParts.Face.Num());
    ModelCustomizations.Add(TEXT("Ears"), ModelParts.Ears.Num());
    ModelCustomizations.Add(TEXT("Hair"), ModelParts.Hair.Num());

    // Do the same for specific parts
    for (const FModelSpecificParts &SpecificPart : ModelParts.SpecificParts)
    {
        ModelPartSelections.Add(SpecificPart.CategoryName, 0);
        ModelCustomizations.Add(SpecificPart.CategoryName, SpecificPart.CategoryParts.Num());
    }
}

FMergedMeshes UCustomizationData::MergeModelParts(const FModelPartSelectionData &ModelPartSelections) const
{
    const FCharacterModelParts &ModelParts = ModelPartSelections.Gender.Equals(TEXT("Male"))
                                                 ? CharacterModels.Find(ModelPartSelections.Race)->MaleModel
                                                 : CharacterModels.Find(ModelPartSelections.Race)->FemaleModel;

    // Create arrays to hold all non-null skeletal meshes
    TArray<USkeletalMesh *> MeshesToMerge;
    TArray<USkeletalMesh *> FP_MeshesToMerge;

    MeshesToMerge.Add(ModelParts.Geoset0);
    FP_MeshesToMerge.Add(ModelParts.Geoset0);

    MeshesToMerge.Add(ModelParts.Torso[ModelPartSelections.UniformIndexes[0]]);
    FP_MeshesToMerge.Add(ModelParts.Torso[ModelPartSelections.UniformIndexes[0]]);

    MeshesToMerge.Add(ModelParts.Gloves[ModelPartSelections.UniformIndexes[1]]);
    FP_MeshesToMerge.Add(ModelParts.Gloves[ModelPartSelections.UniformIndexes[1]]);

    MeshesToMerge.Add(ModelParts.Belt[ModelPartSelections.UniformIndexes[2]]);
    FP_MeshesToMerge.Add(ModelParts.Belt[ModelPartSelections.UniformIndexes[2]]);

    MeshesToMerge.Add(ModelParts.Trousers[ModelPartSelections.UniformIndexes[3]]);
    FP_MeshesToMerge.Add(ModelParts.Trousers[ModelPartSelections.UniformIndexes[3]]);

    MeshesToMerge.Add(ModelParts.Boots[ModelPartSelections.UniformIndexes[4]]);
    FP_MeshesToMerge.Add(ModelParts.Boots[ModelPartSelections.UniformIndexes[4]]);

    MeshesToMerge.Add(ModelParts.Feet[ModelPartSelections.UniformIndexes[5]]);
    FP_MeshesToMerge.Add(ModelParts.Feet[ModelPartSelections.UniformIndexes[5]]);

    MeshesToMerge.Add(ModelParts.Eyes);

    // Add Face mesh based on selection
    MeshesToMerge.Add(ModelParts.Face[ModelPartSelections.UniformIndexes[6]]);

    // Add Ears mesh based on selection
    MeshesToMerge.Add(ModelParts.Ears[ModelPartSelections.UniformIndexes[7]]);

    // Add Hair mesh based on selection (-1 is the value for no mesh)
    if (ModelPartSelections.UniformIndexes[8] != -1)
    {
        for (int32 i = 0; i < ModelParts.Hair[ModelPartSelections.UniformIndexes[8]].Parts.Num(); i++)
        {
            MeshesToMerge.Add(ModelParts.Hair[ModelPartSelections.UniformIndexes[8]].Parts[i]);
        }
    }

    // Process model specific parts - iterate through categories and add selected parts
    for (int32 CategoryIndex = 0; CategoryIndex < ModelParts.SpecificParts.Num(); CategoryIndex++)
    {
        const FModelSpecificParts &Category = ModelParts.SpecificParts[CategoryIndex];

        // Get the selected index for this category from the structured data
        int32 SelectedIndex = ModelPartSelections.SpecificIndexes[ModelPartSelections.SpecificPartNames.Find(Category.CategoryName)];

        if (SelectedIndex == -1)
            continue; // Skip if no part is selected

        for (int32 i = 0; i < Category.CategoryParts[SelectedIndex].Parts.Num(); i++)
        {
            MeshesToMerge.Add(Category.CategoryParts[SelectedIndex].Parts[i]);
        }
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
    Result.AnimBlueprint = ModelParts.AnimBlueprint;

    return Result;
}
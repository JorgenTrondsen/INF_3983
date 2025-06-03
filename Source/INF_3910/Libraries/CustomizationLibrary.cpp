#include "CustomizationLibrary.h"

// Adds a new part selection to the existing model part selections
FModelPartSelectionData UCustomizationLibrary::AddPartSelection(const FModelPartSelectionData &ModelPartSelections, const FString &PartName, int32 SelectionIndex)
{
    FModelPartSelectionData Result = ModelPartSelections;
    Result.Add(PartName, SelectionIndex);
    return Result;
}

// Creates and returns an empty model part selection data structure
FModelPartSelectionData UCustomizationLibrary::CreateEmptyPartSelection()
{
    return FModelPartSelectionData();
}
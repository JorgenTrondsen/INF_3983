#include "CustomizationLibrary.h"

FModelPartSelectionData UCustomizationLibrary::AddPartSelection(const FModelPartSelectionData &ModelPartSelections, const FString &PartName, int32 SelectionIndex)
{
    FModelPartSelectionData Result = ModelPartSelections;
    Result.Add(PartName, SelectionIndex);
    return Result;
}

FModelPartSelectionData UCustomizationLibrary::CreateEmptyPartSelection()
{
    return FModelPartSelectionData();
}
#include "CustomizationData.h"

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

    // Return empty struct if not found
    return FCharacterModelParts();
}
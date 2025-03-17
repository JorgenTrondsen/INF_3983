#include "CustomizationData.h"

FCharacterModelParts UCustomizationData::GetModelParts(ECharacterRace Race, ECharacterGender Gender) const
{
    if (const FRaceModels* RaceModels = CharacterModels.Find(Race))
    {
        if (Gender == ECharacterGender::Male)
        {
            return RaceModels->MaleModel;
        }
        else // Female
        {
            return RaceModels->FemaleModel;
        }
    }
    
    // Return empty struct if not found
    return FCharacterModelParts();
}
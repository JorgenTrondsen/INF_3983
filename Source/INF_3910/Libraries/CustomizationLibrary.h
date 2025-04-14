#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "INF_3910/CharacterCustomization/CustomizationTypes.h"
#include "CustomizationLibrary.generated.h"

/**
 * Blueprint Function Library to expose FModelPartSelectionData functions to blueprints
 */
UCLASS()
class INF_3910_API UCustomizationLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    static FModelPartSelectionData AddPartSelection(const FModelPartSelectionData &ModelPartSelections, const FString &PartName, int32 SelectionIndex);

    UFUNCTION(BlueprintPure, Category = "Character Customization")
    static FModelPartSelectionData CreateEmptyPartSelection();
};
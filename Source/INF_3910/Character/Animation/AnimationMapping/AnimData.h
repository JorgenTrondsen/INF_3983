#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "INFAnimTypes.h"
#include "AnimData.generated.h"

/**
 * Data asset to store animations for all race/gender combinations
 */
UCLASS(BlueprintType)
class INF_3910_API UAnimData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    // Map race to race-specific animations
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = " Animations")
    TMap<FString, FRaceAnimations> RaceAnimations;

    // Get animation montages by race, gender, and type of animation
    UFUNCTION(BlueprintCallable, Category = " Animations")
    TArray<UAnimMontage *> GetMontages(const FString &Race, const FString &Gender, const FString &Type) const;
};
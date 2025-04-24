#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "INFGameInstance.generated.h"

/**
 * Game instance for storing seesion-wide data like map seed
 */
UCLASS()
class INF_3910_API UINFGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UINFGameInstance();

    // Function to get the current map seed
    UFUNCTION(BlueprintCallable, Category = "Map Generation")
    int32 GetMapSeed() const;

    // Function to set a nesetw map seed
    UFUNCTION(BlueprintCallable, Category = "Map Generation")
    void SetMapSeed(int32 NewSeed);

    // Log seed information on init
    virtual void Init() override;

private:
    UPROPERTY()
    int32 MapSeed = 0;
};
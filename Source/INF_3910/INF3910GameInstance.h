// INF_3910 by Jørgen Trondsen, Marcus Ryan and Adrian Moen

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "INF3910GameInstance.generated.h"

/**
 * Game instance for storing seesion-wide data like map seed
 */
UCLASS()
class INF_3910_API UINF3910GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	
public:
	UINF3910GameInstance();
	
	// Function to get the current map seed
    UFUNCTION(BlueprintCallable, Category="Map Generation")
    int32 GetMapSeed() const;
    
    // Function to set a nesetw map seed
    UFUNCTION(BlueprintCallable, Category="Map Generation")
    void SetMapSeed(int32 NewSeed);
    
    // Log seed information on init
    virtual void Init() override;

private:
	UPROPERTY()
	int32 MapSeed = 0;
};
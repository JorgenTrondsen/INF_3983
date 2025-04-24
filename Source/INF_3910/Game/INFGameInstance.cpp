#include "INFGameInstance.h"

UINFGameInstance::UINFGameInstance()
{
    // Initialize with seed 0 (invalid)
    MapSeed = 2;
}

int32 UINFGameInstance::GetMapSeed() const
{
    return MapSeed;
}

void UINFGameInstance::SetMapSeed(int32 NewSeed)
{
    // Only set if the seed is valid
    if (NewSeed != 0)
    {
        MapSeed = NewSeed;
        UE_LOG(LogTemp, Warning, TEXT("Map seed set to: %d"), MapSeed);
    }
}

void UINFGameInstance::Init()
{
    Super::Init();

    // Log the initial seed value
    UE_LOG(LogTemp, Warning, TEXT("Game Instance Initialized. Initial Map Seed: %d"), MapSeed);
}
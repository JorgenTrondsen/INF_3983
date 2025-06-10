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
    }
}

void UINFGameInstance::Init()
{
    Super::Init();
}
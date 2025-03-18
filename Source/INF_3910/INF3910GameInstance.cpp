// INF_3910 by Jørgen Trondsen, Marcus Ryan and Adrian Moen
#include "INF3910GameInstance.h"

UINF3910GameInstance::UINF3910GameInstance()
{
    // Initialize with seed 0 (invalid)
    MapSeed = 2;
}

int32 UINF3910GameInstance::GetMapSeed() const
{
    return MapSeed;
}

void UINF3910GameInstance::SetMapSeed(int32 NewSeed)
{
    // Only set if the seed is valid
    if (NewSeed != 0)
    {
        MapSeed = NewSeed;
        UE_LOG(LogTemp, Warning, TEXT("Map seed set to: %d"), MapSeed);
    }
}

void UINF3910GameInstance::Init()
{
    Super::Init();
    
    // Log the initial seed value
    UE_LOG(LogTemp, Warning, TEXT("Game Instance Initialized. Initial Map Seed: %d"), MapSeed);
}
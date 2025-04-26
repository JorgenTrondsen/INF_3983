#include "INFGameMode.h"
#include "Engine/TimerHandle.h"
#include "INF_3910/Character/INFCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "INF_3910/Game/INFPlayerState.h"                       // Include PlayerState
#include "INF_3910/Character/Customization/SaveCustomization.h" // Include SaveGame class

UCharacterClassInfo *AINFGameMode::GetCharacterClassDefaultInfo() const
{
    return ClassDefaults;
}

UProjectileInfo *AINFGameMode::GetProjectileInfo() const
{
    return ProjectileInfo;
}

void AINFGameMode::PostLogin(APlayerController *NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (AINFPlayerState *PlayerState = NewPlayer->GetPlayerState<AINFPlayerState>())
    {
        if (USaveCustomization *LoadedGame = Cast<USaveCustomization>(UGameplayStatics::LoadGameFromSlot(TEXT("CustomizationSaveSlot"), 0)))
        {
            PlayerState->ModelPartSelectionData = LoadedGame->SavedModelPartSelectionData;
            // Call ApplyAppearanceData here after setting the data on the server
            PlayerState->ApplyAppearanceData();

            UE_LOG(LogTemp, Log, TEXT("Loaded customization for player %s from slot CustomizationSaveSlot and initiated appearance update."), *NewPlayer->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Could not load customization for player %s from slot CustomizationSaveSlot. Using defaults."), *NewPlayer->GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PostLogin: Could not get AINFPlayerState for player %s"), *NewPlayer->GetName());
    }
}

// EVERYTHING UNDER HERE IS FOR MAP_GEN
void AINFGameMode::StartPlay()
{
    // Delay StartPlay to give the map time to replicate
    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, this, &AINFGameMode::DelayedStartPlay, 1.0f, false);

    // Log that we're delaying game start
    UE_LOG(LogTemp, Log, TEXT("GameMode: Delaying game start to allow map replication"));
}

void AINFGameMode::DelayedStartPlay()
{
    Super::StartPlay();
}

AActor *AINFGameMode::FindPlayerStart_Implementation(AController *Player, const FString &IncomingName)
{
    // Debug output to help trace what's happening
    UE_LOG(LogTemp, Log, TEXT("FindPlayerStart called for %s with incoming name: %s"),
           Player ? *Player->GetName() : TEXT("null"), *IncomingName);

    // Look for player starts with the "Player" tag
    TArray<AActor *> TaggedPlayerStarts;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("Player")), TaggedPlayerStarts);

    UE_LOG(LogTemp, Log, TEXT("Found %d tagged player starts"), TaggedPlayerStarts.Num());

    if (TaggedPlayerStarts.Num() > 0)
    {
        // Get player index (for multiplayer)
        int32 PlayerIndex = 0;
        if (Player && Player->PlayerState)
        {
            APlayerState *PS = Cast<APlayerState>(Player->PlayerState);
            if (PS)
            {
                PlayerIndex = PS->GetPlayerId() % TaggedPlayerStarts.Num();
                UE_LOG(LogTemp, Log, TEXT("Player ID: %d, selecting start %d"), PS->GetPlayerId(), PlayerIndex);
            }
        }

        // Return the corresponding player start
        return TaggedPlayerStarts[PlayerIndex];
    }

    // Fall back to default implementation if no custom player starts found
    UE_LOG(LogTemp, Warning, TEXT("No custom player starts found, using default behavior"));
    return Super::FindPlayerStart_Implementation(Player, IncomingName);
}

AActor *AINFGameMode::ChoosePlayerStart_Implementation(AController *Player)
{
    // This will use the FindPlayerStart_Implementation to choose a start
    return Super::ChoosePlayerStart_Implementation(Player);
}
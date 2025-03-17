// Copyright Epic Games, Inc. All Rights Reserved.

#include "INF_3910GameMode.h"
#include "Engine/TimerHandle.h"
#include "INF_3910Character.h"
#include "UObject/ConstructorHelpers.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"

AINF_3910GameMode::AINF_3910GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AINF_3910GameMode::StartPlay()
{
    // Delay StartPlay to give the map time to replicate
    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, this, &AINF_3910GameMode::DelayedStartPlay, 1.0f, false);
    
    // Log that we're delaying game start
    UE_LOG(LogTemp, Log, TEXT("GameMode: Delaying game start to allow map replication"));

}

void AINF_3910GameMode::DelayedStartPlay()
{
    Super::StartPlay();
}

AActor* AINF_3910GameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
   // Debug output to help trace what's happening
   UE_LOG(LogTemp, Log, TEXT("FindPlayerStart called for %s with incoming name: %s"), 
   Player ? *Player->GetName() : TEXT("null"), *IncomingName);

    // Look for player starts with the "Player" tag
    TArray<AActor*> TaggedPlayerStarts;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("Player")), TaggedPlayerStarts);

    UE_LOG(LogTemp, Log, TEXT("Found %d tagged player starts"), TaggedPlayerStarts.Num());

    if (TaggedPlayerStarts.Num() > 0)
    {
        // Get player index (for multiplayer)
        int32 PlayerIndex = 0;
        if (Player && Player->PlayerState)
        {
            APlayerState* PS = Cast<APlayerState>(Player->PlayerState);
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

AActor* AINF_3910GameMode::ChoosePlayerStart_Implementation(AController* Player)
{
    // This will use the FindPlayerStart_Implementation to choose a start
    return Super::ChoosePlayerStart_Implementation(Player);
}
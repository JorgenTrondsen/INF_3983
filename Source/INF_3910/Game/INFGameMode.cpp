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
#include "Engine/World.h"
#include "TimerManager.h"

AINFGameMode::AINFGameMode()
{
    // Enable ticking for win condition checking
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
}

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
        }
      
}

// NEW POI INTEGRATION - BeginPlay
void AINFGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    GameStartTime = GetWorld()->GetTimeSeconds();
    LastWinCheckTime = GameStartTime;
    
    // Find POI in the level with a delay to ensure everything is spawned
    FTimerHandle DelayTimer;
    GetWorldTimerManager().SetTimer(DelayTimer, this, &AINFGameMode::FindAndRegisterPOI, 3.0f, false);
    
    UE_LOG(LogTemp, Log, TEXT("INF Game Mode started - POI game rules active"));
}


// NEW POI INTEGRATION - Tick for win condition checking
void AINFGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    
    if (!bGameEnded)
    {
        float CurrentTime = GetWorld()->GetTimeSeconds();
        
        // Check win conditions at specified interval
        if (CurrentTime - LastWinCheckTime >= WinCheckInterval)
        {
            CheckWinConditions();
            LastWinCheckTime = CurrentTime;
        }
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


// ===== NEW POI INTEGRATION FUNCTIONS =====

void AINFGameMode::FindAndRegisterPOI()
{
    // Find the POI actor in the level
    TArray<AActor*> FoundPOIs;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APOI::StaticClass(), FoundPOIs);
    
    if (FoundPOIs.Num() > 0)
    {
        GamePOI = Cast<APOI>(FoundPOIs[0]);
        UE_LOG(LogTemp, Log, TEXT("Game Mode found and registered POI: %s"), *GamePOI->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No POI found in level! Win conditions may not work properly."));
        
        // Retry after a delay
        FTimerHandle RetryTimer;
        GetWorldTimerManager().SetTimer(RetryTimer, this, &AINFGameMode::FindAndRegisterPOI, 3.0f, false);
    }
}

void AINFGameMode::CheckWinConditions()
{
    if (bGameEnded)
        return;

    APawn* Winner = nullptr;

    // Check score-based win condition
    if (bEnableScoreWin && CheckScoreWinCondition(Winner))
    {
        EndGame(Winner);
        return;
    }

    // Check time-based win condition  
    if (bEnableTimeWin && CheckTimeWinCondition(Winner))
    {
        EndGame(Winner);
        return;
    }
}

bool AINFGameMode::CheckScoreWinCondition(APawn*& OutWinner)
{
    float HighestScore = 0.0f;
    APawn* LeadingPlayer = nullptr;

    // Check all player states for highest score
    for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        APlayerController* PC = Iterator->Get();
        if (PC && PC->GetPawn() && PC->GetPlayerState<APlayerState>())
        {
            float PlayerScore = PC->GetPlayerState<APlayerState>()->GetScore();
            if (PlayerScore >= WinningScore && PlayerScore > HighestScore)
            {
                HighestScore = PlayerScore;
                LeadingPlayer = PC->GetPawn();
            }
        }
    }

    if (LeadingPlayer)
    {
        OutWinner = LeadingPlayer;
        UE_LOG(LogTemp, Warning, TEXT("Score win condition met! Winner: %s with %.0f points"), 
               *LeadingPlayer->GetName(), HighestScore);
        return true;
    }

    return false;
}

bool AINFGameMode::CheckTimeWinCondition(APawn*& OutWinner)
{
    if (!GamePOI)
        return false;

    APawn* CurrentController = GamePOI->GetControllingPlayer();
    float CurrentTime = GetWorld()->GetTimeSeconds();

    if (CurrentController)
    {
        // If this is a new controller, start tracking their time
        if (CurrentController != CurrentTimeWinCandidate)
        {
            CurrentTimeWinCandidate = CurrentController;
            TimeWinStartTime = CurrentTime;
            UE_LOG(LogTemp, Log, TEXT("Started tracking control time for %s"), *CurrentController->GetName());
        }
        else
        {
            // Check if they've controlled long enough
            float ControlDuration = CurrentTime - TimeWinStartTime;
            if (ControlDuration >= ControlTimeToWin)
            {
                OutWinner = CurrentController;
                UE_LOG(LogTemp, Warning, TEXT("Time win condition met! Winner: %s after %.0f seconds of control"), 
                       *CurrentController->GetName(), ControlDuration);
                return true;
            }
        }
    }
    else
    {
        // No one is controlling, reset tracking
        CurrentTimeWinCandidate = nullptr;
        TimeWinStartTime = 0.0f;
    }

    return false;
}

void AINFGameMode::EndGame(APawn* Winner)
{
    if (bGameEnded)
        return;

    bGameEnded = true;
    WinningPlayer = Winner;

    UE_LOG(LogTemp, Warning, TEXT("GAME OVER! Winner: %s"), 
           Winner ? *Winner->GetName() : TEXT("Unknown"));

    // Disable POI scoring
    if (GamePOI)
    {
        GamePOI->SetActorTickEnabled(false);
        UE_LOG(LogTemp, Log, TEXT("POI scoring disabled - game ended"));
    }

    // Disable input for all players
    for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        APlayerController* PC = Iterator->Get();
        if (PC && PC->GetPawn())
        {
            PC->GetPawn()->DisableInput(PC);
        }
    }

    // Call Blueprint event for UI/effects
    OnGameEnded(Winner);

    // // Optionally restart level after delay
    // FTimerHandle RestartTimer;
    // GetWorldTimerManager().SetTimer(RestartTimer, [this]()
    // {
    //     UE_LOG(LogTemp, Log, TEXT("Restarting level..."));
    //     UGameplayStatics::OpenLevel(GetWorld(), FName(*GetWorld()->GetName()));
    // }, 10.0f, false); // Restart after 10 seconds
}
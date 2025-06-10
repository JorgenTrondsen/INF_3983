#include "INFGameMode.h"
#include "Engine/TimerHandle.h"
#include "INF_3910/Character/INFCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
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

// NEW POI INTEGRATION - BeginPlay
void AINFGameMode::BeginPlay()
{
    Super::BeginPlay();

    GameStartTime = GetWorld()->GetTimeSeconds();
    LastWinCheckTime = GameStartTime;

    // Find POI in the level with a delay to ensure everything is spawned
    FTimerHandle DelayTimer;
    GetWorldTimerManager().SetTimer(DelayTimer, this, &AINFGameMode::FindAndRegisterPOI, 3.0f, false);
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
}

void AINFGameMode::DelayedStartPlay()
{
    Super::StartPlay();
}

AActor *AINFGameMode::FindPlayerStart_Implementation(AController *Player, const FString &IncomingName)
{
    // Look for player starts with the "Player" tag
    TArray<AActor *> TaggedPlayerStarts;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("Player")), TaggedPlayerStarts);

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
            }
        }

        // Return the corresponding player start
        return TaggedPlayerStarts[PlayerIndex];
    }

    // Fall back to default implementation if no custom player starts found
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
    TArray<AActor *> FoundPOIs;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APOI::StaticClass(), FoundPOIs);

    if (FoundPOIs.Num() > 0)
    {
        GamePOI = Cast<APOI>(FoundPOIs[0]);
    }
    else
    {
        // Retry after a delay
        FTimerHandle RetryTimer;
        GetWorldTimerManager().SetTimer(RetryTimer, this, &AINFGameMode::FindAndRegisterPOI, 3.0f, false);
    }
}

void AINFGameMode::CheckWinConditions()
{
    if (bGameEnded)
        return;

    APawn *Winner = nullptr;

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

bool AINFGameMode::CheckScoreWinCondition(APawn *&OutWinner)
{
    float HighestScore = 0.0f;
    APawn *LeadingPlayer = nullptr;

    // Check all player states for highest score
    for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        APlayerController *PC = Iterator->Get();
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
        return true;
    }

    return false;
}

bool AINFGameMode::CheckTimeWinCondition(APawn *&OutWinner)
{
    if (!GamePOI)
        return false;

    APawn *CurrentController = GamePOI->GetControllingPlayer();
    float CurrentTime = GetWorld()->GetTimeSeconds();

    if (CurrentController)
    {
        // If this is a new controller, start tracking their time
        if (CurrentController != CurrentTimeWinCandidate)
        {
            CurrentTimeWinCandidate = CurrentController;
            TimeWinStartTime = CurrentTime;
        }
        else
        {
            // Check if they've controlled long enough
            float ControlDuration = CurrentTime - TimeWinStartTime;
            if (ControlDuration >= ControlTimeToWin)
            {
                OutWinner = CurrentController;
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

void AINFGameMode::EndGame(APawn *Winner)
{
    if (bGameEnded)
        return;

    bGameEnded = true;
    WinningPlayer = Winner;

    // Disable POI scoring
    if (GamePOI)
    {
        GamePOI->SetActorTickEnabled(false);
    }

    // Disable input for all players
    for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        APlayerController *PC = Iterator->Get();
        if (PC && PC->GetPawn())
        {
            PC->GetPawn()->DisableInput(PC);
        }
    }

    // Call Blueprint event for UI/effects
    OnGameEnded(Winner);
}
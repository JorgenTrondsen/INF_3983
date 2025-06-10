// INF_3910 by Jørgen Trondsen, Marcus Ryan and Adrian Moen

#include "POI.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"

// Sets default values
APOI::APOI()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Create root component
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    // Create building mesh
    BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BuildingMesh"));
    BuildingMesh->SetupAttachment(RootComponent);

    // Create capture zone
    CaptureZone = CreateDefaultSubobject<USphereComponent>(TEXT("CaptureZone"));
    CaptureZone->SetupAttachment(RootComponent);
    CaptureZone->SetSphereRadius(CaptureRadius);
    CaptureZone->SetCollisionProfileName(TEXT("Trigger"));
    // Hide the visualization of the capture zone
    CaptureZone->SetVisibility(false);
    CaptureZone->SetHiddenInGame(true);

    // Bind collision events
    CaptureZone->OnComponentBeginOverlap.AddDynamic(this, &APOI::OnCaptureZoneBeginOverlap);
    CaptureZone->OnComponentEndOverlap.AddDynamic(this, &APOI::OnCaptureZoneEndOverlap);
}

// Called when the game starts or when spawned
void APOI::BeginPlay()
{
    Super::BeginPlay();

    // Update capture zone radius
    CaptureZone->SetSphereRadius(CaptureRadius);
}

// Called every frame
void APOI::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdatePOIState();
    UpdateCaptureProgress(DeltaTime);

    // Award points to controlling player
    if (CurrentState == EPOIState::Controlled && ControllingPlayer)
    {
        float CurrentTime = GetWorld()->GetTimeSeconds();
        if (CurrentTime - LastScoreTime >= 1.0f)
        {
            AwardPoints(ControllingPlayer, PointsPerSecond);
            LastScoreTime = CurrentTime;
        }
    }
}

void APOI::OnCaptureZoneBeginOverlap(UPrimitiveComponent *OverlappedComponent,
                                     AActor *OtherActor,
                                     UPrimitiveComponent *OtherComp,
                                     int32 OtherBodyIndex,
                                     bool bFromSweep,
                                     const FHitResult &SweepResult)
{
    if (APawn *Player = Cast<APawn>(OtherActor))
    {
        if (Player->IsPlayerControlled())
        {
            PlayersInZone.AddUnique(Player);
            PlayersInZoneCount = PlayersInZone.Num();

            // Use display name
            FString PlayerName = GetPlayerDisplayName(Player);
        }
    }
}

void APOI::OnCaptureZoneEndOverlap(UPrimitiveComponent *OverlappedComponent,
                                   AActor *OtherActor,
                                   UPrimitiveComponent *OtherComp,
                                   int32 OtherBodyIndex)
{
    if (APawn *Player = Cast<APawn>(OtherActor))
    {
        PlayersInZone.Remove(Player);
        PlayersInZoneCount = PlayersInZone.Num();

        // If the controlling player left, lose control immediately
        if (Player == ControllingPlayer)
        {
            OnControlLost();
        }

        // If the capturing player left, reset capture progress
        if (Player == CapturingPlayer)
        {
            CapturingPlayer = nullptr;
            CaptureProgress = 0.0f;
        }
    }
}

void APOI::UpdatePOIState()
{
    // Clean up invalid players
    PlayersInZone.RemoveAll([](APawn *Player)
                            { return !IsValid(Player) || !Player->IsPlayerControlled(); });
    PlayersInZoneCount = PlayersInZone.Num();

    // Update current state based on player count and situation
    if (PlayersInZoneCount == 0)
    {
        // No players in zone
        CurrentState = EPOIState::Neutral;
        CapturingPlayer = nullptr;

        // If there was a controller, they lose control
        if (ControllingPlayer)
        {
            OnControlLost();
        }
    }
    else if (PlayersInZoneCount == 1)
    {
        APawn *SolePlayer = PlayersInZone[0];

        if (SolePlayer == ControllingPlayer)
        {
            // Controlling player is alone in zone
            CurrentState = EPOIState::Controlled;
        }
        else
        {
            // New player is capturing
            CurrentState = EPOIState::Capturing;
            CapturingPlayer = SolePlayer;
        }
    }
    else
    {
        // Multiple players in zone = contested
        CurrentState = EPOIState::Contested;
        CapturingPlayer = nullptr; // Stop any capture in progress
    }
}

void APOI::UpdateCaptureProgress(float DeltaTime)
{
    switch (CurrentState)
    {
    case EPOIState::Neutral:
        // Slowly decay capture progress toward neutral
        CaptureProgress = FMath::Max(0.0f, CaptureProgress - (DeltaTime / CaptureTime) * 0.5f);
        break;

    case EPOIState::Capturing:
        if (CapturingPlayer)
        {
            // Progress toward new controller
            CaptureProgress = FMath::Min(1.0f, CaptureProgress + (DeltaTime / CaptureTime));

            if (CaptureProgress >= 1.0f)
            {
                OnCaptureCompleted(CapturingPlayer);
            }
        }
        break;

    case EPOIState::Controlled:
        // Maintain full capture progress
        CaptureProgress = 1.0f;
        break;

    case EPOIState::Contested:
        // No progress when contested - freeze current progress
        break;
    }
}

void APOI::AwardPoints(APawn *Player, int32 Points)
{
    if (!Player || !Player->GetPlayerState())
        return;

    // Add points to player's score
    Player->GetPlayerState()->SetScore(Player->GetPlayerState()->GetScore() + Points);
}

void APOI::OnCaptureCompleted(APawn *NewController)
{
    APawn *PreviousController = ControllingPlayer;
    ControllingPlayer = NewController;
    CapturingPlayer = nullptr;
    CurrentState = EPOIState::Controlled;
    CaptureProgress = 1.0f;

    // Award capture bonus
    // AwardPoints(NewController, CaptureBonus);

    LastScoreTime = GetWorld()->GetTimeSeconds();
}

void APOI::OnControlLost()
{
    ControllingPlayer = nullptr;
    CaptureProgress = 0.0f;
}

FString APOI::GetPlayerDisplayName(APawn *Player) const
{
    if (!Player || !Player->GetPlayerState())
        return TEXT("Unknown Player");

    // Try to get the player's display name (EIK username)
    FString DisplayName = Player->GetPlayerState()->GetPlayerName();

    // If display name is empty or default, fall back to pawn name
    if (DisplayName.IsEmpty() || DisplayName == TEXT("Player"))
    {
        return Player->GetName();
    }

    return DisplayName;
}

FString APOI::GetControllerDisplayName() const
{
    if (ControllingPlayer)
    {
        return GetPlayerDisplayName(ControllingPlayer);
    }
    return TEXT("None");
}

void APOI::SetCaptureRadius(float NewRadius)
{
    CaptureRadius = NewRadius;
}
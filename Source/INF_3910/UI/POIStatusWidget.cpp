// INF_3910 by Jørgen Trondsen, Marcus Ryan and Adrian Moen


#include "POIStatusWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "GameFramework/Pawn.h"

void UPOIStatusWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    // Enable ticking for real-time updates
    SetIsFocusable(false);
}

void UPOIStatusWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    if (CurrentPOI)
    {
        UpdatePOIDisplay();
    }
}

void UPOIStatusWidget::SetPOI(APOI* InPOI)
{
    CurrentPOI = InPOI;
    
    if (CurrentPOI)
    {
        UE_LOG(LogTemp, Log, TEXT("POI Status Widget connected to POI: %s"), *CurrentPOI->GetName());
    }
}

void UPOIStatusWidget::UpdatePOIDisplay()
{
       if (!CurrentPOI)
        return;

    // Get the owning player for player-specific display
    APawn* LocalPlayer = GetOwningPlayerPawn();
    EPOIState CurrentState = CurrentPOI->GetPOIState();
    
    // Determine player-specific color and text
    FLinearColor StateColor = FLinearColor::White;
    FString StateText = TEXT("POI Status: Neutral");
    
    switch (CurrentState)
    {
        case EPOIState::Neutral:
            StateColor = FLinearColor::White;
            StateText = TEXT("POI Status: Neutral");
            break;
            
        case EPOIState::Capturing:
            if (LocalPlayer == CurrentPOI->GetCapturingPlayer())
            {
                // YOU are capturing - GREEN/YELLOW
                StateColor = FLinearColor::Yellow;
                StateText = FString::Printf(TEXT("POI Status: You're Capturing (%.1f%%)"), 
                           CurrentPOI->GetCaptureProgress() * 100.0f);
            }
            else
            {
                // ENEMY is capturing - RED
                StateColor = FLinearColor::Red;
                StateText = FString::Printf(TEXT("POI Status: Enemy Capturing (%.1f%%)"), 
                           CurrentPOI->GetCaptureProgress() * 100.0f);
            }
            break;
            
        case EPOIState::Controlled:
            if (LocalPlayer == CurrentPOI->GetControllingPlayer())
            {
                StateColor = FLinearColor::Green;
                StateText = TEXT("POI Status: You Control This POI");
            }
            else
            {
                StateColor = FLinearColor::Red;
                StateText = TEXT("POI Status: Enemy Controlled");
            }
            break;
            
        case EPOIState::Contested:
            StateColor = FLinearColor::Red;
            StateText = TEXT("POI Status: Contested");
            break;
    }

    // Apply the player-specific color and text
    if (POIStateText)
    {
        POIStateText->SetText(FText::FromString(StateText));
        POIStateText->SetColorAndOpacity(StateColor);
    }

    // Update progress bar with same color logic
    if (CaptureProgressBar)
    {
        float Progress = CurrentPOI->GetCaptureProgress();
        CaptureProgressBar->SetPercent(Progress);
        CaptureProgressBar->SetFillColorAndOpacity(StateColor);
    }

    // Rest of your existing code...
    if (ControllingPlayerText)
    {
        FString ControllerName = CurrentPOI->GetControllerDisplayName();
        FString ControllerText = FString::Printf(TEXT("Controlled by: %s"), *ControllerName);
        ControllingPlayerText->SetText(FText::FromString(ControllerText));
    }

    if (PlayersInZoneText)
    {
        int32 PlayerCount = CurrentPOI->GetPlayersInZone();
        FString CountText = FString::Printf(TEXT("Players in Zone: %d"), PlayerCount);
        PlayersInZoneText->SetText(FText::FromString(CountText));
    }
}

FString UPOIStatusWidget::GetStateDisplayText(EPOIState State)
{
    switch (State)
    {
        case EPOIState::Neutral: return TEXT("Neutral");
        case EPOIState::Capturing: return TEXT("Capturing");
        case EPOIState::Controlled: return TEXT("Controlled");
        case EPOIState::Contested: return TEXT("Contested");
        default: return TEXT("Unknown");
    }
}

FLinearColor UPOIStatusWidget::GetStateColor(EPOIState State)
{
    switch (State)
    {
        case EPOIState::Neutral: return FLinearColor::White;
        case EPOIState::Capturing: return FLinearColor::Yellow;
        case EPOIState::Controlled: return FLinearColor::Green;
        case EPOIState::Contested: return FLinearColor::Red;
        default: return FLinearColor::White;
    }
}
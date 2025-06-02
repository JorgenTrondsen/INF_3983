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

    // Update POI State
    EPOIState CurrentState = CurrentPOI->GetPOIState();
    FString StateText = FString::Printf(TEXT("POI Status: %s"), *GetStateDisplayText(CurrentState));
    
    if (POIStateText)
    {
        POIStateText->SetText(FText::FromString(StateText));
        POIStateText->SetColorAndOpacity(GetStateColor(CurrentState));
    }

    // Update Capture Progress
    if (CaptureProgressBar)
    {
        float Progress = CurrentPOI->GetCaptureProgress();
        CaptureProgressBar->SetPercent(Progress);
        
        // Change color based on state
        FLinearColor BarColor = GetStateColor(CurrentState);
        CaptureProgressBar->SetFillColorAndOpacity(BarColor);
    }

    // Update Controlling Player
    if (ControllingPlayerText)
    {
        APawn* Controller = CurrentPOI->GetControllingPlayer();
        FString ControllerText = Controller ? 
            FString::Printf(TEXT("Controlled by: %s"), *Controller->GetName()) :
            TEXT("Controlled by: None");
        
        ControllingPlayerText->SetText(FText::FromString(ControllerText));
    }

    // Update Players in Zone
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
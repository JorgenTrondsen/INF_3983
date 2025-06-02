// INF_3910 by Jørgen Trondsen, Marcus Ryan and Adrian Moen


#include "PlayerScoreWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"


void UPlayerScoreWidget::NativeConstruct()
{
    Super::NativeConstruct();
    SetIsFocusable(false);
}

void UPlayerScoreWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    UpdatePlayerScore();
}

void UPlayerScoreWidget::UpdatePlayerScore()
{
    // Get the owning player's score
    if (APlayerController* PC = GetOwningPlayer())
    {
        if (APlayerState* PS = PC->GetPlayerState<APlayerState>())
        {
            float CurrentScore = PS->GetScore();
            
            if (PlayerScoreText)
            {
                FString ScoreText = FString::Printf(TEXT("Your Score: %.0f"), CurrentScore);
                PlayerScoreText->SetText(FText::FromString(ScoreText));
            }
            
            LastKnownScore = CurrentScore;
        }
    }
}
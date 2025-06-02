// INF_3910 by Jørgen Trondsen, Marcus Ryan and Adrian Moen

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "PlayerScoreWidget.generated.h"


UCLASS()
class INF_3910_API UPlayerScoreWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Score")
    void UpdatePlayerScore();

protected:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* PlayerScoreText;

private:
    float LastKnownScore = 0.0f;
	
};

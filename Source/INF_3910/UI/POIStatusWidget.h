// INF_3910 by Jørgen Trondsen, Marcus Ryan and Adrian Moen

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "INF_3910/POI/POI.h"
#include "POIStatusWidget.generated.h"

class APOI;

UCLASS()
class INF_3910_API UPOIStatusWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // Initialize the widget
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    // Set the POI reference
    UFUNCTION(BlueprintCallable, Category = "POI")
    void SetPOI(APOI* InPOI);

    // Update functions
    UFUNCTION(BlueprintCallable, Category = "POI")
    void UpdatePOIDisplay();

protected:
	protected:
    // Widget components (bind to Blueprint widgets)
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* POIStateText;

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* CaptureProgressBar;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ControllingPlayerText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* PlayersInZoneText;

    // POI reference
    UPROPERTY(BlueprintReadOnly, Category = "POI")
    APOI* CurrentPOI = nullptr;

private:
	// Helper functions
    FString GetStateDisplayText(EPOIState State);
    FLinearColor GetStateColor(EPOIState State);
};

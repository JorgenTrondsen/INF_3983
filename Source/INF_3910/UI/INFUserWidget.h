#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "INFUserWidget.generated.h"

class UWidgetController;
/**
 *
 */
UCLASS()
class INF_3910_API UINFUserWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetWidgetController(UWidgetController *InWidgetController);

    UFUNCTION(BlueprintImplementableEvent)
    void OnWidgetControllerSet();

private:
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<UWidgetController> WidgetController;
};
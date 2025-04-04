#include "INFUserWidget.h"

void UINFUserWidget::SetWidgetController(UWidgetController *InWidgetController)
{
    WidgetController = InWidgetController;
    OnWidgetControllerSet();
}
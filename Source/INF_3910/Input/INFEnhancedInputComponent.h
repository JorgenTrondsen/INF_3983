#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "INFInputConfig.h"
#include "INFEnhancedInputComponent.generated.h"

class UINFInputConfig;
/**
 *
 */
UCLASS()
class INF_3910_API UINFEnhancedInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(UINFInputConfig *InputConfig, UserClass *Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc);
};

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UINFEnhancedInputComponent::BindAbilityActions(UINFInputConfig *InputConfig, UserClass *Object,
													PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc)
{
	check(InputConfig);

	for (const FINFInputAction &Action : InputConfig->INFInputActions)
	{
		if (IsValid(Action.InputAction) && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			}
			if (ReleasedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
			}
		}
	}
}
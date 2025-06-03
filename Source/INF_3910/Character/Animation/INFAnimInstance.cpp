#include "INFAnimInstance.h"

// Sets up the property map to bind animation properties with the ability system for data-driven animations.
void UINFAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent *ASC)
{
    check(ASC);

    PropertyMap.Initialize(this, ASC);
}
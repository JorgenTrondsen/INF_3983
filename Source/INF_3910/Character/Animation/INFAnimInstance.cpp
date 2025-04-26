#include "INFAnimInstance.h"

void UINFAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent *ASC)
{
    check(ASC);

    PropertyMap.Initialize(this, ASC);
}
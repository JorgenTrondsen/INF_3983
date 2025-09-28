#include "INFGameMode.h"
#include "Engine/TimerHandle.h"
#include "INF_3910/Character/INFCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "Engine/World.h"
#include "TimerManager.h"

UCharacterClassInfo *AINFGameMode::GetCharacterClassDefaultInfo() const
{
    return ClassDefaults;
}

UProjectileInfo *AINFGameMode::GetProjectileInfo() const
{
    return ProjectileInfo;
}

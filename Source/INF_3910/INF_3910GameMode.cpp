// Copyright Epic Games, Inc. All Rights Reserved.

#include "INF_3910GameMode.h"
#include "INF_3910Character.h"
#include "UObject/ConstructorHelpers.h"

AINF_3910GameMode::AINF_3910GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

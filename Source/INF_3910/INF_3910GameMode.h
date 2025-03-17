// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "INF_3910GameMode.generated.h"

UCLASS(minimalapi)
class AINF_3910GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AINF_3910GameMode();

	virtual void StartPlay() override;

	void DelayedStartPlay();

	// Override FindPlayerStart to implement custom player start logic
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	
private:
	// Keep track of which player starts have been used
	TArray<AActor*> UsedPlayerStarts;
};



#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "INFGameMode.generated.h"

class UProjectileInfo;
class UCharacterClassInfo;
/**
 *
 */
UCLASS()
class INF_3910_API AINFGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	UCharacterClassInfo *GetCharacterClassDefaultInfo() const;
	UProjectileInfo *GetProjectileInfo() const;

	// FROM MAP_GEN GAMEMODE V
	virtual void StartPlay() override;
	void DelayedStartPlay();
	// Override FindPlayerStart to implement custom player start logic
	virtual AActor *FindPlayerStart_Implementation(AController *Player, const FString &IncomingName) override;
	virtual AActor *ChoosePlayerStart_Implementation(AController *Player) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Class Defaults")
	TObjectPtr<UCharacterClassInfo> ClassDefaults;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Projectiles")
	TObjectPtr<UProjectileInfo> ProjectileInfo;

	// FROM MAP_GEN GAMEMODE
	// Keep track of which player starts have been used
	TArray<AActor *> UsedPlayerStarts;
};

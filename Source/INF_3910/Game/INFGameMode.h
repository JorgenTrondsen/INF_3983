#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include"INF_3910/POI/POI.h"
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
	AINFGameMode();

	UCharacterClassInfo *GetCharacterClassDefaultInfo() const;
	UProjectileInfo *GetProjectileInfo() const;

	virtual void PostLogin(APlayerController *NewPlayer) override;

	// FROM MAP_GEN GAMEMODE V
	virtual void StartPlay() override;
	void DelayedStartPlay();
	// Override FindPlayerStart to implement custom player start logic
	virtual AActor *FindPlayerStart_Implementation(AController *Player, const FString &IncomingName) override;
	virtual AActor *ChoosePlayerStart_Implementation(AController *Player) override;

	// === POI GAME MODE INTEGRATION ===
    
    // Win condition settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rules")
    float WinningScore = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rules")
    float ControlTimeToWin = 300.0f; // 5 minutes of continuous control

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rules")
    bool bEnableScoreWin = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rules")
    bool bEnableTimeWin = false; // Disabled by default for free-for-all

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rules")
    float WinCheckInterval = 1.0f; // Check win conditions every second

    // Game state tracking
    UPROPERTY(BlueprintReadOnly, Category = "Game State")
    bool bGameEnded = false;

    UPROPERTY(BlueprintReadOnly, Category = "Game State")
    APawn* WinningPlayer = nullptr;

    // POI reference
    UPROPERTY(BlueprintReadOnly, Category = "POI")
    APOI* GamePOI = nullptr;

    // Functions
    UFUNCTION(BlueprintCallable, Category = "Game Management")
    void CheckWinConditions();

    UFUNCTION(BlueprintCallable, Category = "Game Management")
    void EndGame(APawn* Winner);

    UFUNCTION(BlueprintImplementableEvent, Category = "Game Events")
    void OnGameEnded(APawn* Winner);

    UFUNCTION(BlueprintImplementableEvent, Category = "Game Events")
    void OnPlayerScoreChanged(APawn* Player, float NewScore);

    // Override to enable tick
    virtual void Tick(float DeltaSeconds) override;	


protected:
	// Override BeginPlay to set up POI tracking
    virtual void BeginPlay() override;

    // Find and register the POI in the level
    void FindAndRegisterPOI();

    // Internal win condition checks
    bool CheckScoreWinCondition(APawn*& OutWinner);
    bool CheckTimeWinCondition(APawn*& OutWinner);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Class Defaults")
	TObjectPtr<UCharacterClassInfo> ClassDefaults;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Projectiles")
	TObjectPtr<UProjectileInfo> ProjectileInfo;

	// FROM MAP_GEN GAMEMODE
	// Keep track of which player starts have been used
	TArray<AActor *> UsedPlayerStarts;

	 // POI game tracking
    float GameStartTime = 0.0f;
    float LastWinCheckTime = 0.0f;

    // Time-based win tracking
    APawn* CurrentTimeWinCandidate = nullptr;
    float TimeWinStartTime = 0.0f;
};

// INF_3910 by Jørgen Trondsen, Marcus Ryan and Adrian Moen

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/Engine.h"
#include "POI.generated.h"

UENUM(BlueprintType)
enum class EPOIState : uint8
{
	Neutral     UMETA(DisplayName = "Neutral"),
    Capturing   UMETA(DisplayName = "Capturing"),
    Controlled  UMETA(DisplayName = "Controlled"),
    Contested   UMETA(DisplayName = "Contested")
};

UCLASS()
class INF_3910_API APOI : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APOI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// === COMPONENTS ===
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* BuildingMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* CaptureZone;

	// === POI SETTINGS ===
    UPROPERTY(EditAnywhere, Category = "POI Settings")
    float CaptureRadius = 500.0f;

    UPROPERTY(EditAnywhere, Category = "POI Settings")
    float CaptureTime = 5.0f; // Seconds to capture (faster for FFA)

    UPROPERTY(EditAnywhere, Category = "POI Settings")
    int32 PointsPerSecond = 2; // Points awarded while controlling

    UPROPERTY(EditAnywhere, Category = "POI Settings")
    int32 CaptureBonus = 10; // Bonus points for successful capture

	// === POI STATE ===
    UPROPERTY(BlueprintReadOnly, Category = "POI State")
    EPOIState CurrentState = EPOIState::Neutral;

    UPROPERTY(BlueprintReadOnly, Category = "POI State")
    APawn* ControllingPlayer = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "POI State")
    float CaptureProgress = 0.0f; // 0.0 to 1.0

    UPROPERTY(BlueprintReadOnly, Category = "POI State")
    int32 PlayersInZoneCount = 0;

	// === COLLISION EVENTS ===
    UFUNCTION()
    void OnCaptureZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent,
								   AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp,
								   int32 OtherBodyIndex,
                                   bool bFromSweep,
								   const FHitResult& SweepResult);

    UFUNCTION()
    void OnCaptureZoneEndOverlap(UPrimitiveComponent* OverlappedComponent,
								 AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp,
								 int32 OtherBodyIndex);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// === PUBLIC INTERFACE ===
    UFUNCTION(BlueprintCallable, Category = "POI")
    EPOIState GetPOIState() const { return CurrentState; }

    UFUNCTION(BlueprintCallable, Category = "POI")
    APawn* GetControllingPlayer() const { return ControllingPlayer; }

    UFUNCTION(BlueprintCallable, Category = "POI")
    float GetCaptureProgress() const { return CaptureProgress; }

    UFUNCTION(BlueprintCallable, Category = "POI")
    int32 GetPlayersInZone() const { return PlayersInZoneCount; }
	
private:
	// === INTERNAL STATE ===
    TArray<APawn*> PlayersInZone;
    APawn* CapturingPlayer = nullptr; // Player currently capturing
    float LastScoreTime = 0.0f;

    // === INTERNAL FUNCTIONS ===
    void UpdatePOIState();
    void UpdateCaptureProgress(float DeltaTime);
    void AwardPoints(APawn* Player, int32 Points);
    void UpdateVisualState();
    void OnCaptureCompleted(APawn* NewController);
    void OnControlLost();
};


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "INF_3910/AbilitySystem/AbilityTypes.h"
#include "ProjectileBase.generated.h"

class USphereComponent;
struct FProjectileParams;
class UProjectileMovementComponent;

UCLASS()
class INF_3910_API AProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	AProjectileBase();

	void SetProjectileParams(const FProjectileParams &Params);

	UPROPERTY(BlueprintReadWrite)
	FDamageEffectInfo DamageEffectInfo;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

private:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> OverlapSphere;

	UPROPERTY()
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;
};
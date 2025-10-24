#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Logging/LogMacros.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ObjectPtr.h"
#include "PCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class INF_3910_API APCharacter : public ABaseCharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent *CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent *FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext *DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *LookAction;

	// First-person components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent *FP_Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent *FP_Camera;

public:
	APCharacter();

	virtual void PossessedBy(AController *NewController) override;
	virtual void OnRep_PlayerState() override;

	// Override to support first-person mesh
	virtual USkeletalMeshComponent *GetFirstPersonMesh() const override { return FP_Mesh; }

protected:
	void Move(const FInputActionValue &Value);
	void Look(const FInputActionValue &Value);

	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;
	virtual void BeginPlay() override;

	// Override base class methods
	virtual void InitAbilityActorInfo() override;
	virtual void ApplyDeadSettings() override;
	virtual void ApplyAliveSettings() override;

public:
	FORCEINLINE class USpringArmComponent *GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent *GetFollowCamera() const { return FollowCamera; }
};

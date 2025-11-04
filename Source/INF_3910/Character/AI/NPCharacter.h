#pragma once

#include "CoreMinimal.h"
#include "INF_3910/Character/BaseCharacter.h"
#include "INF_3910/Character/Customization/CustomizationTypes.h"
#include "INF_3910/Interfaces/InteractInterface.h"
#include "NPCharacter.generated.h"

class UDialogueWidgetController;
class UINFUserWidget;
class UAnimData;

/**
 * NPC Character class that inherits from BaseCharacter
 * Provides NPC-specific functionality with Gameplay Ability System support
 */
UCLASS()
class INF_3910_API ANPCharacter : public ABaseCharacter, public IInteractInterface
{
    GENERATED_BODY()

public:
    ANPCharacter();

    virtual void PossessedBy(AController *NewController) override;
    virtual void BeginPlay() override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

    // Display name for this NPC (shown in dialogue and UI)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    FString DisplayName;

    UFUNCTION(BlueprintCallable)
    void CreateDialogueWidget(APlayerController *PlayerController, UUserWidget *Widget);

    UFUNCTION(BlueprintCallable)
    void FacePlayer(APlayerController *PlayerController);

    UFUNCTION(BlueprintCallable)
    void StopFacingPlayer();

    UPROPERTY(BlueprintReadWrite, Category = "NPC")
    bool bIsInDialogue = false;

    UPROPERTY(BlueprintReadOnly, Category = "NPC")
    float PitchRotation;

protected:
    // Override initialization to support NPC-specific setup
    virtual void InitAbilityActorInfo() override;

    // Static appearance configuration for NPCs (configured per-instance in editor)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_NPCAppearanceData, Category = "Character Customization")
    FModelPartSelectionData NPCAppearanceData;

    UFUNCTION()
    void OnRep_NPCAppearanceData();

    UPROPERTY()
    TObjectPtr<UDialogueWidgetController> DialogueWidgetController;

    UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Widgets")
    TSubclassOf<UDialogueWidgetController> DialogueWidgetControllerClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom Values|Widgets")
    TSubclassOf<UINFUserWidget> DialogueWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
    TObjectPtr<UAnimData> AnimDataAsset;

private:
    UDialogueWidgetController *GetDialogueWidgetController(APlayerController *PlayerController);
};
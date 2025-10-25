#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "INF_3910/Character/Customization/CustomizationTypes.h"
#include "INF_3910/Interfaces/InteractableInterface.h"
#include "NPCharacter.generated.h"

/**
 * NPC Character class that inherits from BaseCharacter
 * Provides NPC-specific functionality with Gameplay Ability System support
 */
UCLASS()
class INF_3910_API ANPCharacter : public ABaseCharacter, public IInteractableInterface
{
    GENERATED_BODY()

public:
    ANPCharacter();

    virtual void PossessedBy(AController *NewController) override;
    virtual void BeginPlay() override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

    // IInteractableInterface
    virtual void OnInteract_Implementation(AActor *InteractingActor) override;

protected:
    // Override initialization to support NPC-specific setup
    virtual void InitAbilityActorInfo() override;

    // Static appearance configuration for NPCs (configured per-instance in editor)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_NPCAppearanceData, Category = "Character Customization")
    FModelPartSelectionData NPCAppearanceData;

    UFUNCTION()
    void OnRep_NPCAppearanceData();
};
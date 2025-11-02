#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ItemTypes.generated.h"

class UEquipmentDefinition;
class UGameplayEffect;
class UEquipmentInstance;
class AEquipmentActor;

USTRUCT(BlueprintType)
struct FEquipmentItemProps
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UEquipmentDefinition> EquipmentClass = nullptr;
};

USTRUCT(BlueprintType)
struct FConsumableProps
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UGameplayEffect> ItemEffectClass = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float ItemEffectLevel = 1.f;
};

USTRUCT(BlueprintType)
struct FMasterItemDefinition : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayTag ItemTag = FGameplayTag();

    UPROPERTY(BlueprintReadOnly)
    int32 ItemQuantity = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FText ItemName = FText();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FText Description = FText();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSoftObjectPtr<UStaticMesh> StaticItemMesh = nullptr;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSoftObjectPtr<USkeletalMesh> SkeletalItemMesh = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSoftObjectPtr<UTexture2D> Icon = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FConsumableProps ConsumableProps = FConsumableProps();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FEquipmentItemProps EquipmentItemProps = FEquipmentItemProps();
};
#include "BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "INF_3910/Libraries/AbilitySystemLibrary.h"
#include "INF_3910/AbilitySystem/CharacterClassInfo.h"
#include "INF_3910/Character/Customization/CustomizationData.h"
#include "INF_3910/Character/Animation/INFAnimInstance.h"
#include "Net/UnrealNetwork.h"

// Constructor that initializes base character components
ABaseCharacter::ABaseCharacter()
{
    // Setup dynamic projectile spawn point
    DynamicProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnPoint"));
    DynamicProjectileSpawnPoint->SetupAttachment(GetMesh(), ProjectileSpawnSocketName);

    // Initialize dead state
    bIsDead = false;
}

// Returns the ability system component for this character
UAbilitySystemComponent *ABaseCharacter::GetAbilitySystemComponent() const
{
    return INFAbilitySystemComp;
}

// Returns the dynamic projectile spawn point for projectile spawning interface
USceneComponent *ABaseCharacter::GetDynamicSpawnPoint_Implementation()
{
    return DynamicProjectileSpawnPoint;
}

// Updates character appearance based on customization data and model part selections
void ABaseCharacter::UpdateAppearance(const FModelPartSelectionData &ModelPartSelections)
{
    if (!CustomizationData)
    {
        UE_LOG(LogTemp, Warning, TEXT("CustomizationData asset is not assigned in %s"), *GetNameSafe(this));
        return;
    }

    const FMergedMeshes MergedMeshes = CustomizationData->MergeModelParts(ModelPartSelections);

    GetMesh()->SetAnimClass(MergedMeshes.AnimBlueprint);
    GetMesh()->SetSkeletalMeshAsset(MergedMeshes.ThirdPersonMesh);

    // Apply first-person mesh if this character has one
    if (USkeletalMeshComponent *FP_Mesh = GetFirstPersonMesh())
    {
        FP_Mesh->SetAnimClass(MergedMeshes.AnimBlueprint);
        FP_Mesh->SetSkeletalMeshAsset(MergedMeshes.FirstPersonMesh);
    }

    if (IsValid(INFAbilitySystemComp))
    {
        if (UINFAnimInstance *INFAnimInstance = Cast<UINFAnimInstance>(GetMesh()->GetAnimInstance()))
        {
            INFAnimInstance->InitializeWithAbilitySystem(INFAbilitySystemComp);
        }
    }
}

// Initializes ability system component and attributes - to be overridden by child classes
void ABaseCharacter::InitAbilityActorInfo()
{
    // Base implementation - override in child classes
}

// Initializes default abilities and attributes based on character class
void ABaseCharacter::InitClassDefaults()
{
    if (!CharacterTag.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("No Character Tag Selected In This Character %s"), *GetNameSafe(this));
        return;
    }

    if (UCharacterClassInfo *ClassInfo = UAbilitySystemLibrary::GetCharacterClassDefaultInfo(this))
    {
        if (const FCharacterClassDefaultInfo *SelectedClassInfo = ClassInfo->ClassDefaultInfoMap.Find(CharacterTag))
        {
            if (IsValid(INFAbilitySystemComp))
            {
                INFAbilitySystemComp->AddCharacterAbilities(SelectedClassInfo->StartingAbilities);
                INFAbilitySystemComp->AddCharacterPassiveAbilities(SelectedClassInfo->StartingPassives);
                INFAbilitySystemComp->InitializeDefaultAttributes(SelectedClassInfo->DefaultAttributes);
            }
        }
    }
}

// Binds attribute change callbacks for health and stamina updates
void ABaseCharacter::BindCallbacksToDependencies()
{
    if (IsValid(INFAbilitySystemComp) && IsValid(INFAttributes))
    {
        // Clear existing delegates first to prevent multiple bindings from recreated characters
        INFAbilitySystemComp->GetGameplayAttributeValueChangeDelegate(INFAttributes->GetHealthAttribute()).Clear();
        INFAbilitySystemComp->GetGameplayAttributeValueChangeDelegate(INFAttributes->GetStaminaAttribute()).Clear();

        INFAbilitySystemComp->GetGameplayAttributeValueChangeDelegate(INFAttributes->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData &Data)
                                                                                                                     { OnHealthChanged(Data.NewValue, INFAttributes->GetMaxHealth()); });

        INFAbilitySystemComp->GetGameplayAttributeValueChangeDelegate(INFAttributes->GetStaminaAttribute()).AddLambda([this](const FOnAttributeChangeData &Data)
                                                                                                                      { OnStaminaChanged(Data.NewValue, INFAttributes->GetMaxStamina()); });
    }
}

// Broadcasts initial attribute values for UI initialization
void ABaseCharacter::BroadcastInitialValues()
{
    if (IsValid(INFAttributes))
    {
        OnHealthChanged(INFAttributes->GetHealth(), INFAttributes->GetMaxHealth());
        OnStaminaChanged(INFAttributes->GetStamina(), INFAttributes->GetMaxStamina());
    }
}

// Registers properties for network replication
void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ABaseCharacter, bIsDead);
}

// Applies settings when character dies (disable input, enable physics simulation)
void ABaseCharacter::ApplyDeadSettings()
{
    GetMesh()->SetAllBodiesSimulatePhysics(true);
    GetMesh()->SetSimulatePhysics(true);
    GetMesh()->WakeAllRigidBodies();

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
    GetCharacterMovement()->StopMovementImmediately();
    GetCharacterMovement()->DisableMovement();
}

// Applies settings when character is alive (enable input, disable physics simulation)
void ABaseCharacter::ApplyAliveSettings()
{
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

    GetMesh()->SetAllBodiesSimulatePhysics(false);
    GetMesh()->SetSimulatePhysics(false);

    const FVector DefaultRelativeLocation = FVector(0.f, 0.f, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
    const FRotator DefaultRelativeRotation = FRotator(0.f, 0.f, 0.f);
    GetMesh()->SetRelativeLocationAndRotation(DefaultRelativeLocation, DefaultRelativeRotation);
}

// Called when dead state replicates to apply appropriate settings
void ABaseCharacter::OnRep_IsDead()
{
    if (bIsDead)
    {
        ApplyDeadSettings();
    }
    else
    {
        ApplyAliveSettings();
    }
}

// Sets the dead state and triggers replication (server only)
void ABaseCharacter::SetDeadState(bool bNewIsDead)
{
    if (HasAuthority())
    {
        bIsDead = bNewIsDead;
        OnRep_IsDead();
    }
}

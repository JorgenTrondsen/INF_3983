#include "INFCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "INF_3910/Libraries/INFAbilitySystemLibrary.h"
#include "INF_3910/AbilitySystem/CharacterClassInfo.h"
#include "INF_3910/Game/INFPlayerState.h"
#include "INF_3910/AbilitySystem/INFAbilitySystemComponent.h"
#include "INF_3910/AbilitySystem/INFAttributeSet.h"
#include "INF_3910/Character/Animation/INFAnimInstance.h"
#include "UObject/Object.h"
#include "INF_3910/Character/Customization/CustomizationData.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AINFCharacter::AINFCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	DynamicProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(("ProjectileSpawnPoint"));
	DynamicProjectileSpawnPoint->SetupAttachment(GetRootComponent());

	FP_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FP_Mesh->SetupAttachment(GetMesh());
	FP_Mesh->SetLeaderPoseComponent(GetMesh());
}

void AINFCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AINFCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	if (APlayerController *PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if (UEnhancedInputComponent *EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AINFCharacter::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AINFCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AINFCharacter::Move(const FInputActionValue &Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AINFCharacter::Look(const FInputActionValue &Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AINFCharacter::UpdateAppearance(const FModelPartSelectionData &ModelPartSelections)
{
	if (!CustomizationData)
	{
		UE_LOG(LogTemp, Warning, TEXT("CustomizationData asset is not assigned in %s"), *GetNameSafe(this));
		return;
	}

	const FMergedMeshes MergedMeshes = CustomizationData->MergeModelParts(ModelPartSelections);

	GetMesh()->SetAnimClass(MergedMeshes.AnimBlueprint);
	GetMesh()->SetSkeletalMeshAsset(MergedMeshes.ThirdPersonMesh);

	if (FP_Mesh)
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

USceneComponent *AINFCharacter::GetDynamicSpawnPoint_Implementation()
{
	return DynamicProjectileSpawnPoint;
}

void AINFCharacter::PossessedBy(AController *NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();
}

void AINFCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();
}

UAbilitySystemComponent *AINFCharacter::GetAbilitySystemComponent() const
{
	return INFAbilitySystemComp;
}

void AINFCharacter::InitAbilityActorInfo()
{
	if (AINFPlayerState *INFPlayerState = GetPlayerState<AINFPlayerState>())
	{
		INFAbilitySystemComp = Cast<UINFAbilitySystemComponent>(INFPlayerState->GetAbilitySystemComponent());
		INFAttributes = INFPlayerState->GetINFAttributes();

		if (IsValid(INFAbilitySystemComp))
		{
			INFAbilitySystemComp->InitAbilityActorInfo(INFPlayerState, this);
			BindCallbacksToDependencies();

			if (HasAuthority())
			{
				InitClassDefaults();
			}
		}
	}
}

void AINFCharacter::InitClassDefaults()
{
	if (!CharacterTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Character Tag Selected In This Character %s"), *GetNameSafe(this));
	}
	else if (UCharacterClassInfo *ClassInfo = UINFAbilitySystemLibrary::GetCharacterClassDefaultInfo(this))
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
void AINFCharacter::BindCallbacksToDependencies()
{
	if (IsValid(INFAbilitySystemComp) && IsValid(INFAttributes))
	{
		INFAbilitySystemComp->GetGameplayAttributeValueChangeDelegate(INFAttributes->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData &Data)
																													 { OnHealthChanged(Data.NewValue, INFAttributes->GetMaxHealth()); });

		INFAbilitySystemComp->GetGameplayAttributeValueChangeDelegate(INFAttributes->GetStaminaAttribute()).AddLambda([this](const FOnAttributeChangeData &Data)
																													  { OnStaminaChanged(Data.NewValue, INFAttributes->GetMaxStamina()); });
	}
}

void AINFCharacter::BroadcastInitialValues()
{
	if (IsValid(INFAttributes))
	{
		OnHealthChanged(INFAttributes->GetHealth(), INFAttributes->GetMaxHealth());
		OnStaminaChanged(INFAttributes->GetStamina(), INFAttributes->GetMaxStamina());
	}
}

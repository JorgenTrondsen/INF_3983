#include "PCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "INF_3910/Libraries/AbilitySystemLibrary.h"
#include "INF_3910/AbilitySystem/CharacterClassInfo.h"
#include "INF_3910/Game/INFPlayerState.h"
#include "INF_3910/AbilitySystem/INFAbilitySystemComponent.h"
#include "INF_3910/AbilitySystem/INFAttributeSet.h"
#include "INF_3910/Character/Animation/INFAnimInstance.h"
#include "UObject/Object.h"
#include "INF_3910/Character/Customization/CustomizationData.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

// Constructor that initializes character components and default settings
APCharacter::APCharacter()
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

	DynamicProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnPoint"));
	DynamicProjectileSpawnPoint->SetupAttachment(GetMesh(), ProjectileSpawnSocketName);

	GetMesh()->SetOwnerNoSee(true);

	FP_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FP_Mesh->SetupAttachment(GetMesh());
	FP_Mesh->SetLeaderPoseComponent(GetMesh());
	FP_Mesh->SetOnlyOwnerSee(true);
	FP_Mesh->SetCastShadow(false);

	FP_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FP_Camera->SetupAttachment(FP_Mesh, TEXT("bone_EXP_C1_Neck1Socket"));
	FP_Camera->bUsePawnControlRotation = true;
}

// Called when the game starts or when spawned
void APCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
		return;

	if (APlayerController *PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

// Sets up input mapping context and action bindings
void APCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
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

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APCharacter::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APCharacter::Look);
	}
}

// Handles character movement input based on forward and right directions
void APCharacter::Move(const FInputActionValue &Value)
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

// Handles camera look input for yaw and pitch rotation
void APCharacter::Look(const FInputActionValue &Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

// Updates character appearance based on customization data and model part selections
void APCharacter::UpdateAppearance(const FModelPartSelectionData &ModelPartSelections)
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

// Returns the dynamic projectile spawn point for projectile spawning interface
USceneComponent *APCharacter::GetDynamicSpawnPoint_Implementation()
{
	return DynamicProjectileSpawnPoint;
}

// Called when character is possessed by a controller on the server
void APCharacter::PossessedBy(AController *NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();

	if (AINFPlayerState *PS = GetPlayerState<AINFPlayerState>())
	{
		UpdateAppearance(PS->ModelPartSelectionData);
	}
}

// Called when player state is replicated to clients
void APCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();

	if (AINFPlayerState *PS = GetPlayerState<AINFPlayerState>())
	{
		UpdateAppearance(PS->ModelPartSelectionData);
	}
}

// Returns the ability system component for this character
UAbilitySystemComponent *APCharacter::GetAbilitySystemComponent() const
{
	return INFAbilitySystemComp;
}

// Initializes ability system component and attributes from player state
void APCharacter::InitAbilityActorInfo()
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

// Initializes default abilities and attributes based on character class
void APCharacter::InitClassDefaults()
{
	if (!CharacterTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Character Tag Selected In This Character %s"), *GetNameSafe(this));
	}
	else if (UCharacterClassInfo *ClassInfo = UAbilitySystemLibrary::GetCharacterClassDefaultInfo(this))
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
void APCharacter::BindCallbacksToDependencies()
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
void APCharacter::BroadcastInitialValues()
{
	if (IsValid(INFAttributes))
	{
		OnHealthChanged(INFAttributes->GetHealth(), INFAttributes->GetMaxHealth());
		OnStaminaChanged(INFAttributes->GetStamina(), INFAttributes->GetMaxStamina());
	}
}

// Registers properties for network replication
void APCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APCharacter, bIsDead);
}

// Applies settings when character dies (disable input, enable physics simulation)
void APCharacter::ApplyDeadSettings()
{
	if (APlayerController *PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}

	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
}

// Applies settings when character is alive (enable input, disable physics simulation)
void APCharacter::ApplyAliveSettings()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	GetMesh()->SetAllBodiesSimulatePhysics(false);
	GetMesh()->SetSimulatePhysics(false);

	const FVector DefaultRelativeLocation = FVector(0.f, 0.f, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
	const FRotator DefaultRelativeRotation = FRotator(0.f, 0.f, 0.f);
	GetMesh()->SetRelativeLocationAndRotation(DefaultRelativeLocation, DefaultRelativeRotation);

	if (APlayerController *PC = Cast<APlayerController>(GetController()))
	{
		EnableInput(PC);
	}
}

// Called when dead state replicates to apply appropriate settings
void APCharacter::OnRep_IsDead()
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
void APCharacter::SetDeadState(bool bNewIsDead)
{
	if (HasAuthority())
	{
		bIsDead = bNewIsDead;
		OnRep_IsDead();
	}
}

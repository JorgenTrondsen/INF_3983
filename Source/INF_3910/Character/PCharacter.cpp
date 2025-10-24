#include "PCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "INF_3910/Game/INFPlayerState.h"
#include "INF_3910/AbilitySystem/INFAbilitySystemComponent.h"
#include "INF_3910/AbilitySystem/INFAttributeSet.h"

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

// Applies settings when character dies (disable input for player)
void APCharacter::ApplyDeadSettings()
{
	Super::ApplyDeadSettings();

	if (APlayerController *PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}
}

// Applies settings when character is alive (enable input for player)
void APCharacter::ApplyAliveSettings()
{
	Super::ApplyAliveSettings();

	if (APlayerController *PC = Cast<APlayerController>(GetController()))
	{
		EnableInput(PC);
	}
}

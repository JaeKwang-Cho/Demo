// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/DemoPlayerCharacterBase.h"


#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AI/PlayerAIController.h"
#include "Player/DemoPlayerState.h"
#include "Player/DemoPlayerController.h"
#include "Character/AbilitySystem/CharacterAbilitySystemComponent.h"
#include "Character/AbilitySystem/AttributeSets/CharacterAttributeSetBase.h"


//////////////////////////////////////////////////////////////////////////
// ADemoCharacter

ADemoPlayerCharacterBase::ADemoPlayerCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(FName("NoCollision"));

	AIControllerClass = APlayerAIController::StaticClass();

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	StunTag = FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun"));
}

void ADemoPlayerCharacterBase::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	StartingCameraBoomArmLength = CameraBoom->TargetArmLength;
	StartingCameraBoomLocation = CameraBoom->GetRelativeLocation();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ADemoPlayerCharacterBase::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ADemoPlayerCharacterBase::HandleJumpActionPressed);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ADemoPlayerCharacterBase::HandleJumpActionReleased);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADemoPlayerCharacterBase::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADemoPlayerCharacterBase::Look);

		//Abilities
		EnhancedInputComponent->BindAction(Ability1Action, ETriggerEvent::Triggered, this, &ADemoPlayerCharacterBase::HandleAbility1ActionPressed);
		EnhancedInputComponent->BindAction(Ability2Action, ETriggerEvent::Triggered, this, &ADemoPlayerCharacterBase::HandleAbility2ActionPressed);
		EnhancedInputComponent->BindAction(Ability3Action, ETriggerEvent::Triggered, this, &ADemoPlayerCharacterBase::HandleAbility3ActionPressed);
		EnhancedInputComponent->BindAction(Ability4Action, ETriggerEvent::Triggered, this, &ADemoPlayerCharacterBase::HandleAbility4ActionPressed);
		EnhancedInputComponent->BindAction(Ability5Action, ETriggerEvent::Triggered, this, &ADemoPlayerCharacterBase::HandleAbility5ActionPressed);
		EnhancedInputComponent->BindAction(Ability6Action, ETriggerEvent::Triggered, this, &ADemoPlayerCharacterBase::HandleAbility6ActionPressed);

		EnhancedInputComponent->BindAction(Ability1Action, ETriggerEvent::Completed, this, &ADemoPlayerCharacterBase::HandleAbility1ActionReleased);
		EnhancedInputComponent->BindAction(Ability2Action, ETriggerEvent::Completed, this, &ADemoPlayerCharacterBase::HandleAbility2ActionReleased);
		EnhancedInputComponent->BindAction(Ability3Action, ETriggerEvent::Completed, this, &ADemoPlayerCharacterBase::HandleAbility3ActionReleased);
		EnhancedInputComponent->BindAction(Ability4Action, ETriggerEvent::Completed, this, &ADemoPlayerCharacterBase::HandleAbility4ActionReleased);
		EnhancedInputComponent->BindAction(Ability5Action, ETriggerEvent::Completed, this, &ADemoPlayerCharacterBase::HandleAbility5ActionReleased);
		EnhancedInputComponent->BindAction(Ability6Action, ETriggerEvent::Completed, this, &ADemoPlayerCharacterBase::HandleAbility6ActionReleased);

		// mouse
		EnhancedInputComponent->BindAction(ConfirmAction, ETriggerEvent::Triggered, this, &ADemoPlayerCharacterBase::HandleConfirmActionPressed);
		EnhancedInputComponent->BindAction(ConfirmAction, ETriggerEvent::Triggered, this, &ADemoPlayerCharacterBase::HandleConfirmActionReleased);

		EnhancedInputComponent->BindAction(CancelAction, ETriggerEvent::Completed, this, &ADemoPlayerCharacterBase::HandleCancelActionPressed);
		EnhancedInputComponent->BindAction(CancelAction, ETriggerEvent::Completed, this, &ADemoPlayerCharacterBase::HandleCancelActionReleased);

		BindASCInput();
	}

}

void ADemoPlayerCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ADemoPlayerState* PS = GetPlayerState< ADemoPlayerState>();
	if (PS)
	{
		InitializeStartingValues(PS);

		AddStartupEffects();
		AddCharacterAbilities();
	}
}

void ADemoPlayerCharacterBase::OnRep_PlayerState()
{ 
	Super::OnRep_PlayerState();

	ADemoPlayerState* PS = GetPlayerState< ADemoPlayerState>();
	if (PS)
	{
		InitializeStartingValues(PS);
		BindASCInput();
		InitializeAttributes();
	}
}

void ADemoPlayerCharacterBase::InitializeStartingValues(ADemoPlayerState* PS)
{
	AbilitySystemComponent = Cast<UCharacterAbilitySystemComponent>(PS->GetAbilitySystemComponent());

	AbilitySystemComponent.Get()->InitAbilityActorInfo(PS, this);

	AttributeSetBase = PS->GetAttributeSetBase();

	AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

	InitializeAttributes();

	SetHealth(GetMaxHealth());
	SetMana(GetMaxMana());
}



void ADemoPlayerCharacterBase::BindASCInput()
{
	if (!ASCInputBound && AbilitySystemComponent.IsValid() && IsValid(InputComponent))
	{
		AbilitySystemComponent->BindAbilityActivationToInputComponent(
			InputComponent, 
			FGameplayAbilityInputBinds(
				FString("ConfirmTarget"), 
				FString("CancelTarget"), 
				FString("DemoAbilityID"),
				static_cast<int32>(DemoAbilityID::Confirm), 
				static_cast<int32>(DemoAbilityID::Cancel)
			)
		);
	}

	ASCInputBound = true;
}

void ADemoPlayerCharacterBase::SendLocalInputToASC(bool bIsPressed, const DemoAbilityID AbilityInputID)
{
	if (!AbilitySystemComponent.IsValid())
	{
		return;
	}

	if (bIsPressed)
	{
		AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(AbilityInputID));
	}
	else
	{
		AbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(AbilityInputID));
	}
}

void ADemoPlayerCharacterBase::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr && AbilitySystemComponent->GetGameplayTagCount(StunTag) == 0)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ADemoPlayerCharacterBase::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ADemoPlayerCharacterBase::HandleJumpActionPressed()
{
	SendLocalInputToASC(true, DemoAbilityID::Jump);
	Jump();
}

void ADemoPlayerCharacterBase::HandleJumpActionReleased()
{
	SendLocalInputToASC(false, DemoAbilityID::Jump);
	StopJumping();
}


/** Called for Abilities input */
void ADemoPlayerCharacterBase::HandleAbility1ActionPressed()
{
	//UE_LOG(LogTemp, Warning, TEXT("ABility 1"));
	SendLocalInputToASC(true, DemoAbilityID::Ability1);
}

void ADemoPlayerCharacterBase::HandleAbility1ActionReleased()
{
	SendLocalInputToASC(false, DemoAbilityID::Ability1);
}

void ADemoPlayerCharacterBase::HandleAbility2ActionPressed()
{
	//UE_LOG(LogTemp, Warning, TEXT("ABility 2"));
	SendLocalInputToASC(true, DemoAbilityID::Ability2);
}

void ADemoPlayerCharacterBase::HandleAbility2ActionReleased()
{
	SendLocalInputToASC(false, DemoAbilityID::Ability2);
}

void ADemoPlayerCharacterBase::HandleAbility3ActionPressed()
{
	//UE_LOG(LogTemp, Warning, TEXT("ABility 3"));
	SendLocalInputToASC(true, DemoAbilityID::Ability3);
}

void ADemoPlayerCharacterBase::HandleAbility3ActionReleased()
{
	SendLocalInputToASC(false, DemoAbilityID::Ability3);
}

void ADemoPlayerCharacterBase::HandleAbility4ActionPressed()
{
	//UE_LOG(LogTemp, Warning, TEXT("ABility 4"));
	SendLocalInputToASC(true, DemoAbilityID::Ability4);
}

void ADemoPlayerCharacterBase::HandleAbility4ActionReleased()
{
	SendLocalInputToASC(false, DemoAbilityID::Ability4);
}

void ADemoPlayerCharacterBase::HandleAbility5ActionPressed()
{
	//UE_LOG(LogTemp, Warning, TEXT("ABility 5"));
	SendLocalInputToASC(true, DemoAbilityID::Ability5);
}

void ADemoPlayerCharacterBase::HandleAbility5ActionReleased()
{
	SendLocalInputToASC(false, DemoAbilityID::Ability5);
}

void ADemoPlayerCharacterBase::HandleAbility6ActionPressed()
{
	//UE_LOG(LogTemp, Warning, TEXT("ABility 6"));
	SendLocalInputToASC(true, DemoAbilityID::Ability6);
}

void ADemoPlayerCharacterBase::HandleAbility6ActionReleased()
{
	SendLocalInputToASC(false, DemoAbilityID::Ability6);
}

void ADemoPlayerCharacterBase::HandleConfirmActionPressed()
{
	//UE_LOG(LogTemp, Warning, TEXT("Confirm"));
	SendLocalInputToASC(true, DemoAbilityID::Confirm);
}

void ADemoPlayerCharacterBase::HandleConfirmActionReleased()
{
	SendLocalInputToASC(false, DemoAbilityID::Confirm);
}

void ADemoPlayerCharacterBase::HandleCancelActionPressed()
{
	//UE_LOG(LogTemp, Warning, TEXT("Canel"));
	SendLocalInputToASC(true, DemoAbilityID::Cancel);
}

void ADemoPlayerCharacterBase::HandleCancelActionReleased()
{
	SendLocalInputToASC(false, DemoAbilityID::Cancel);
}

float ADemoPlayerCharacterBase::GetStartingCameraBoomArmLength()
{
	return StartingCameraBoomArmLength;
}

FVector ADemoPlayerCharacterBase::GetStartingCameraBoomLocation()
{
	return StartingCameraBoomLocation;
}
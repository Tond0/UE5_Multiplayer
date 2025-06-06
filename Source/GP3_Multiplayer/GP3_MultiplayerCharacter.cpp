// Copyright Epic Games, Inc. All Rights Reserved.

#include "GP3_MultiplayerCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Public/Mechanics/SizeChangerComponent.h"
#include "Public/Mechanics/InteractableBoxComponent.h"
#include "Math/UnrealMathVectorConstants.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AGP3_MultiplayerCharacter

AGP3_MultiplayerCharacter::AGP3_MultiplayerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
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
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	SizeComponent = CreateDefaultSubobject<USizeChangerComponent>(TEXT("Size Changer"));
	SizeComponent->SetIsReplicated(true);

	bReplicates = true;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

FDelegateHandle AGP3_MultiplayerCharacter::BindToOnInteractableChanged(UObject* Object, FName FunctionName)
{
	return OnInteractableChanged.AddUFunction(Object, FunctionName);
}

void AGP3_MultiplayerCharacter::UnbindToOnInteractableChanged(FDelegateHandle Handle)
{
	OnInteractableChanged.Remove(Handle);
}

bool AGP3_MultiplayerCharacter::BindToOnPowerActionPerformed(UObject* Object, FName FunctionName)
{
	//If this delegate is already bound, then you can't change the function is pointing to.
	//The first function to bind will be always be the same (Security reason).
	if (OnPowerActionPerformed.IsBound())
		return false;

	OnPowerActionPerformed.BindUFunction(Object, FunctionName);
	return true;
}

void AGP3_MultiplayerCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

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

void AGP3_MultiplayerCharacter::SetTargetPowerState_Implementation(EPowerState NewTargetPowerState)
{
	if (!HasAuthority()) return;

	SizeComponent->TargetPowerState = NewTargetPowerState;
}

void AGP3_MultiplayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGP3_MultiplayerCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGP3_MultiplayerCharacter::Look);

		// Power 
		EnhancedInputComponent->BindAction(PowerAction, ETriggerEvent::Completed, this, &AGP3_MultiplayerCharacter::Server_PerfomPower);

		// Interaction
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &AGP3_MultiplayerCharacter::Server_Interact);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AGP3_MultiplayerCharacter::Server_Interact_Implementation(const FInputActionValue& Value)
{
	if (!HasAuthority()) return;

	if (!Interactable.GetObject()) return;

	//Lol il nome della funzione per colpa di unreal è diventato Execute_Execute
	Interactable->Execute_Execute(Interactable.GetObject());
}

bool AGP3_MultiplayerCharacter::TryReplaceInteractable(TScriptInterface<IExecutable> NewInteractable)
{
	if (!NewInteractable.GetObject()) return false;

	//If there's already an interactable...
	if (Interactable.GetObject())
	{
		//... We decide which one to keep depending on the squared distance.
		float InteractableSqrdDist = GetSquaredDistanceTo(Cast<AActor>(Interactable.GetObject()));
		float NewInteractableSqrdDist = GetSquaredDistanceTo(Cast<AActor>(NewInteractable.GetObject()));
		//If its closer, we choose the new one.
		if (NewInteractableSqrdDist < InteractableSqrdDist)
		{
			Interactable = NewInteractable;
			OnInteractableChanged.Broadcast(this, Interactable);
			return true;
		}
	}
	else
	{
		Interactable = NewInteractable;
		//FIXME: This one it's useless because if it is was nullptr then there's no other interactable fighting to be the main one for this player.
		OnInteractableChanged.Broadcast(this, Interactable);
		return true;
	}

	return false;
}

void AGP3_MultiplayerCharacter::RemoveInteractable(TScriptInterface<IExecutable> InteractableToRemove)
{
	if (InteractableToRemove.GetObject() != Interactable.GetObject()) return;

	Interactable = nullptr;
	//FIXME: Old method, now every box component, but the main one, are checking every X seconds.
	////We start by setting it to nullptr
	//Interactable = nullptr;

	////Check if we're already overlapping with any actor with the UInteractableBoxComponent.
	//TSet<AActor*> OverlappingInteractables;
	//GetOverlappingActors(OverlappingInteractables, UInteractableBoxComponent::StaticClass());

	////The min squared distance we are looking for.
	//float MinSquaredDistance = INFINITY;
	//AActor* ClosestInteractableActor = nullptr;

	////Let's check which is the closest
	//for (AActor* CurrentOverlappingInteractable : OverlappingInteractables)
	//{
	//	float CurrentSquaredDistance = GetSquaredDistanceTo(CurrentOverlappingInteractable);

	//	if (CurrentSquaredDistance < MinSquaredDistance)
	//	{
	//		MinSquaredDistance = CurrentSquaredDistance;
	//		ClosestInteractableActor = CurrentOverlappingInteractable;
	//	}
	//}

	//if (ClosestInteractableActor)
	//{
	//	//Assign new closest interactable.
	//	Interactable = ClosestInteractableActor;
	//	//Any Executable binded (close to player will be notified)
	//	OnInteractableChanged.Broadcast(Interactable);
	//}
}

void AGP3_MultiplayerCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
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

void AGP3_MultiplayerCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AGP3_MultiplayerCharacter::Server_PerfomPower_Implementation(const FInputActionValue& Value)
{
	OnPowerActionPerformed.ExecuteIfBound();
}
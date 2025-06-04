// Fill out your copyright notice in the Description page of Project Settings.


#include "Mechanics/SizeChangerComponent.h"
#include "GP3_Multiplayer/GP3_MultiplayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
USizeChangerComponent::USizeChangerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

// Called when the game starts
void USizeChangerComponent::BeginPlay()
{
	Super::BeginPlay();

	//Bind to action delegate.
	AActor* Owner = GetOwner();
	AGP3_MultiplayerCharacter* Character = Cast<AGP3_MultiplayerCharacter>(Owner);
	Character->BindToOnPowerActionPerformed(this, "TogglePowerState");

	//Let's remember the standard JumpZVelocity.
	MovementComponent = Cast<UCharacterMovementComponent>(Character->GetMovementComponent());

	//Save standard settings
	StandardPowerSettings = FStatePowerSettings(
		FVector(1),
		MovementComponent->JumpZVelocity,
		MovementComponent->GravityScale,
		MovementComponent->MaxWalkSpeed
	);

	//This should never happen...
	if (TargetPowerState == EPowerState::Standard)
		//... but if this happen a log error is shown.
		UE_LOG(LogTemp, Error, TEXT("TargetState is standard, something went wrong"));
}

void USizeChangerComponent::ChangePowerState_Implementation(EPowerState NextPowerState)
{
	AActor* ActorOwner = GetOwner();

	switch (NextPowerState)
	{
	case EPowerState::Standard:
		ApplyNewSettings(ActorOwner, StandardPowerSettings);
		break;

	case EPowerState::Big:
		ApplyNewSettings(ActorOwner, BigPowerSettings);
		break;

	case EPowerState::Small:
		ApplyNewSettings(ActorOwner, SmallPowerSettings);
		break;
	}

	CurrentPowerState = NextPowerState;
}

void USizeChangerComponent::ApplyNewSettings(AActor* ActorOwner, FStatePowerSettings SettingsToApply)
{
	ActorOwner->SetActorScale3D(SettingsToApply.Size);
	MovementComponent->JumpZVelocity = SettingsToApply.JumpZVelocity;
	MovementComponent->GravityScale = SettingsToApply.GravityScale;
	MovementComponent->MaxWalkSpeed = SettingsToApply.MaxWalkSpeed;
}

void USizeChangerComponent::TogglePowerState()
{
	//This is supposed to run on the server only.
	if (!GetOwner()->HasAuthority()) return;

	//Switching while jumping will reset the velocity to 0
	//Prevent jump with standard force and switch to low gravity for really, really, really big jumps.
	if (MovementComponent->Velocity.Z >= 0 && !MovementComponent->IsMovingOnGround())
		MovementComponent->Velocity.Z = 0;

	//Toggle the states.
	if (CurrentPowerState == TargetPowerState)
		ChangePowerState(EPowerState::Standard);
	else
		ChangePowerState(TargetPowerState);
}

void USizeChangerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USizeChangerComponent, CurrentPowerState);
}


// Called every frame
void USizeChangerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


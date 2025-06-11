// Fill out your copyright notice in the Description page of Project Settings.


#include "Mechanics/SizeChangerComponent.h"
#include "GP3_Multiplayer/GP3_MultiplayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
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
void USizeChangerComponent::SetUp_Implementation(AGP3_MultiplayerCharacter* Character)
{
	//We don't need clients to bind to the delegate, they will change size on the replication of CurrentState.
	if (Character->HasAuthority()) 
		//Bind to action delegate.
		Character->BindToOnPowerActionPerformed(this, "TogglePowerState");

	MovementComponent = Cast<UCharacterMovementComponent>(Character->GetMovementComponent());
	CapsuleComponent = Character->GetCapsuleComponent();

	//Save standard settings
	StandardPowerSettings = FStatePowerSettings(
		FVector(1),
		MovementComponent->JumpZVelocity,
		MovementComponent->GravityScale,
		MovementComponent->MaxWalkSpeed
	);
}

void USizeChangerComponent::ChangePowerState_Implementation(EPowerState NextPowerState)
{
	AActor* ActorOwner = GetOwner();

	if (!ActorOwner->HasAuthority()) return;

	switch (NextPowerState)
	{
	case EPowerState::Standard:
		//Can we come back to normal if we were small?
		if (TargetPowerState == EPowerState::Small && !CheckIfScalable(StandardPowerSettings.Size)) return;
		ApplyNewSettings(ActorOwner, StandardPowerSettings);
		break;

	case EPowerState::Big:
		//Can we grow?
		if (!CheckIfScalable(BigPowerSettings.Size)) return;
		ApplyNewSettings(ActorOwner, BigPowerSettings);
		break;

	case EPowerState::Small:
		ApplyNewSettings(ActorOwner, SmallPowerSettings);
		break;
	}

	CurrentPowerState = NextPowerState;
	OnSizeChanged.Broadcast(CurrentPowerState);
}

void USizeChangerComponent::ApplyNewSettings(AActor* ActorOwner, FStatePowerSettings SettingsToApply)
{
	//If we're growing...
	if (SettingsToApply.Size.Length() > ActorOwner->GetActorScale3D().Length())
		//We move the player upwards for a short distance to avoid clipping with the floor.
		ActorOwner->SetActorLocation(ActorOwner->GetActorLocation() + FVector(0, 0, 20));
	ActorOwner->SetActorScale3D(SettingsToApply.Size);

	//If we were moving upwards then we stop the momentum, to avoid jumping with a force and then switch to a low gravity for higher jumps.
	if (MovementComponent->Velocity.Z >= 0 && !MovementComponent->IsMovingOnGround())
		MovementComponent->Velocity.Z = 0;

	MovementComponent->JumpZVelocity = SettingsToApply.JumpZVelocity;
	MovementComponent->GravityScale = SettingsToApply.GravityScale;
	MovementComponent->MaxWalkSpeed = SettingsToApply.MaxWalkSpeed;
}

bool USizeChangerComponent::CheckIfScalable(FVector SizeToCheck)
{
	FHitResult OutHit = FHitResult();

	FVector SweepStart = GetOwner()->GetActorLocation() + FVector(0,0,60);
	//Sul posto
	FVector SweepEnd = SweepStart;

	FQuat Rotation = FQuat(GetOwner()->GetActorRotation());

	float CapsuleShapeRadius = CapsuleComponent->GetUnscaledCapsuleRadius() * SizeToCheck.X;
	float CapsuleShapeHalfHeight = CapsuleComponent->GetUnscaledCapsuleHalfHeight() * SizeToCheck.Z;
	FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CapsuleShapeRadius, CapsuleShapeHalfHeight);

	FCollisionQueryParams QueryParams = FCollisionQueryParams();
	QueryParams.AddIgnoredActor(GetOwner());

	DrawDebugCapsule(GetWorld(), SweepStart, CapsuleShapeHalfHeight, CapsuleShapeRadius, Rotation, FColor::Green, false, 2, 0, 2);
	//Se collidiamo con qualcosa allora non possiamo ingrandirci!
	return !GetWorld()->SweepSingleByChannel(OutHit, SweepStart, SweepEnd, Rotation, ECollisionChannel::ECC_Camera, CapsuleShape, QueryParams);
}

void USizeChangerComponent::TogglePowerState_Implementation()
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
	DOREPLIFETIME(USizeChangerComponent, TargetPowerState);
}


void USizeChangerComponent::OnRep_CurrentPowerState()
{
	AActor* ActorOwner = GetOwner();

	switch (CurrentPowerState)
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
}

// Called every frame
void USizeChangerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


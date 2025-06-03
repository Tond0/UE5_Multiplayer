// Fill out your copyright notice in the Description page of Project Settings.


#include "Mechanics/SizeChangerComponent.h"
#include "GP3_Multiplayer/GP3_MultiplayerCharacter.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
USizeChangerComponent::USizeChangerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}


void USizeChangerComponent::ToggleSize_Implementation()
{
	AActor* ActorOwner = GetOwner();

	//In which state is the player in?
	if (isAlteringSize)
	{
		//Come back to normal size
		ActorOwner->SetActorScale3D(FVector(1));
	}
	else
	{
		//Become bigger/smaller.
		ActorOwner->SetActorScale3D(TargetSize);
	}

	isAlteringSize = !isAlteringSize;
}

void USizeChangerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USizeChangerComponent, isAlteringSize);
	DOREPLIFETIME(USizeChangerComponent, TargetSize);
}

// Called when the game starts
void USizeChangerComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	AGP3_MultiplayerCharacter* Character = Cast<AGP3_MultiplayerCharacter>(Owner);
	Character->BindToOnPowerActionPerformed(this, "ToggleSize");
}


// Called every frame
void USizeChangerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


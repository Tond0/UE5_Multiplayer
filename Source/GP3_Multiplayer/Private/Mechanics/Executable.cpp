// Fill out your copyright notice in the Description page of Project Settings.


#include "Mechanics/Executable.h"
#include "Net/UnrealNetwork.h"
#include "Mechanics/InteractableBoxComponent.h"

// Sets default values
AExecutable::AExecutable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AExecutable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExecutable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExecutable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AExecutable, CanExecute);
}

void AExecutable::DisableExecutable()
{
	if(HasAuthority())
		CanExecute = false;

	//Disable any interaction we might have with the player.
	if (UInteractableBoxComponent* InteractableBox = GetComponentByClass<UInteractableBoxComponent>())
	{
		//FIXME: Basta?
		InteractableBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AExecutable::Execute_Implementation()
{
	if (!CanExecute) return;

	//Only the server will start the execution, even if this method is NetMulticast.
	//if(HasAuthority())
		ExecuteOnTargets();

	if (ExecuteOnce)
		DisableExecutable();
}

void AExecutable::ExecuteOnTargets()
{
	for (AExecutable* CurrentTargetExecutable : TargetExecutables)
	{
		CurrentTargetExecutable->Execute();
	}
}

void AExecutable::Interact_Implementation()
{
 	Execute();
}


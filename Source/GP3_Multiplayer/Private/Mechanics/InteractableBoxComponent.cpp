// Fill out your copyright notice in the Description page of Project Settings.


#include "Mechanics/InteractableBoxComponent.h"
#include "GP3_Multiplayer/GP3_MultiplayerCharacter.h"

UInteractableBoxComponent::UInteractableBoxComponent()
{
	SetIsReplicatedByDefault(true);
	SetIsReplicated(true);
}

void UInteractableBoxComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddUniqueDynamic(this, &UInteractableBoxComponent::OnOverlapBegin);
	OnComponentEndOverlap.AddUniqueDynamic(this, &UInteractableBoxComponent::OnOverlapEnd);
}

void UInteractableBoxComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if its the character (the collision preset will be set to only overlap with the player anyway)
	AGP3_MultiplayerCharacter* CharacterOverlapping = Cast<AGP3_MultiplayerCharacter>(OtherActor);
	if (!CharacterOverlapping) return;

	CharacterOverlapping->ReplaceInteractable(this);
}

void UInteractableBoxComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Check if its the character (the collision preset will be set to only overlap with the player anyway)
	AGP3_MultiplayerCharacter* CharacterOverlapping = Cast<AGP3_MultiplayerCharacter>(OtherActor);
	if (!CharacterOverlapping) return;

	//Try to remove itself.
	CharacterOverlapping->RemoveInteractable(this);
}


template <typename UserClass>
void UInteractableBoxComponent::BindToOnInteracted(UserClass* Owner, void (UserClass::* FuncName)())
{
	OnInteracted.AddUniqueDynamic(this, FuncName);
}
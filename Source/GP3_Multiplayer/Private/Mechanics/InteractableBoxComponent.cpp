// Fill out your copyright notice in the Description page of Project Settings.


#include "Mechanics/InteractableBoxComponent.h"
#include "GP3_Multiplayer/GP3_MultiplayerCharacter.h"
#include "Components/WidgetComponent.h"
#include "Mechanics/Interactable.h"

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

	InteractableOwner = GetOwner();

	USceneComponent* FirstChild = GetChildComponent(0);
	if (FirstChild)
	{
		if(InteractableWidgetComponent = Cast<UWidgetComponent>(FirstChild))
			InteractableWidgetComponent->SetVisibility(false, false);
	}
}

void UInteractableBoxComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if its the character (the collision preset will be set to only overlap with the player anyway)
	AGP3_MultiplayerCharacter* CharacterOverlapping = Cast<AGP3_MultiplayerCharacter>(OtherActor);
	if (!CharacterOverlapping) return;

	CharacterOverlapping->ReplaceInteractable(InteractableOwner);
	
	HandleOnInteractableChanged = CharacterOverlapping->BindToOnInteractableChanged(this, "Handle_OnInteractableChanged");

	if (InteractableWidgetComponent && CharacterOverlapping->GetInteractable() == GetOwner())
		InteractableWidgetComponent->SetVisibility(true, true);
}

void UInteractableBoxComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Check if its the character (the collision preset will be set to only overlap with the player anyway)
	AGP3_MultiplayerCharacter* CharacterOverlapping = Cast<AGP3_MultiplayerCharacter>(OtherActor);
	if (!CharacterOverlapping) return;

	//Try to remove itself.
	CharacterOverlapping->RemoveInteractable(InteractableOwner);

	CharacterOverlapping->UnbindToOnInteractableChanged(HandleOnInteractableChanged);

	if (InteractableWidgetComponent)
		InteractableWidgetComponent->SetVisibility(false, false);
}

void UInteractableBoxComponent::Handle_OnInteractableChanged(TScriptInterface<IExecutable> NewInteractable)
{
	if (NewInteractable != GetOwner()) return;

	if (InteractableWidgetComponent)
		InteractableWidgetComponent->SetVisibility(true, true);
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Mechanics/InteractableBoxComponent.h"
#include "GP3_Multiplayer/GP3_MultiplayerCharacter.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
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
		InteractableWidgetComponent = Cast<UWidgetComponent>(FirstChild);
		if(InteractableWidgetComponent)
			InteractableWidgetComponent->SetVisibility(false, false);
	}
}

void UInteractableBoxComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if its the character (the collision preset will be set to only overlap with the player anyway)
	AGP3_MultiplayerCharacter* CharacterOverlapping = Cast<AGP3_MultiplayerCharacter>(OtherActor);
	if (!CharacterOverlapping) return;

	//Try to set itself as main interactable for the player
	if (CharacterOverlapping->TryReplaceInteractable(InteractableOwner))
		//We are the interactable the player could interact with in this moment.
		SetAsMainInteractable(CharacterOverlapping);
	else
		//We are the interactable the player can't interact with, but it's in range.
		SetAsSideInteractable(CharacterOverlapping);
	
	HandleOnInteractableChanged = CharacterOverlapping->BindToOnInteractableChanged(this, "Handle_OnInteractableChanged");
}

void UInteractableBoxComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Check if its the character (the collision preset will be set to only overlap with the player anyway)
	AGP3_MultiplayerCharacter* CharacterOverlapping = Cast<AGP3_MultiplayerCharacter>(OtherActor);
	if (!CharacterOverlapping) return;

	//Removes itself
	CharacterOverlapping->RemoveInteractable(InteractableOwner);
	//Player is not in range anymore.
	SetAsSleepInteractable(CharacterOverlapping);

	CharacterOverlapping->UnbindToOnInteractableChanged(HandleOnInteractableChanged);
}

void UInteractableBoxComponent::SetAsMainInteractable(AGP3_MultiplayerCharacter* CharacterOverlapping)
{
	//Were we checking to be the main interactable?
	if(TimerCheckHandle.IsValid())
		//We are the main interactable, we don't need to still checking.
		GetWorld()->GetTimerManager().ClearTimer(TimerCheckHandle);

	//Turn on widget
	if (InteractableWidgetComponent && CharacterOverlapping->IsLocallyControlled())
		InteractableWidgetComponent->SetVisibility(true);
}

void UInteractableBoxComponent::SetAsSideInteractable(AGP3_MultiplayerCharacter* CharacterOverlapping)
{
	//Create delegate function to call each CheckRate seconds
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("CheckValidInteractable"), CharacterOverlapping);
	GetWorld()->GetTimerManager().SetTimer(TimerCheckHandle, TimerDelegate, CheckRateSeconds, true);

	//Turn off widget, only on the local client
	if (InteractableWidgetComponent  && CharacterOverlapping->IsLocallyControlled())
		InteractableWidgetComponent->SetVisibility(false);
}

void UInteractableBoxComponent::SetAsSleepInteractable(AGP3_MultiplayerCharacter* CharacterOverlapping)
{
	//Were we checking to be the main interactable?
	if (TimerCheckHandle.IsValid())
		//We are the main interactable, we don't need to still checking.
		GetWorld()->GetTimerManager().ClearTimer(TimerCheckHandle);

	//Turn off widget
	if (InteractableWidgetComponent && CharacterOverlapping->IsLocallyControlled())
		InteractableWidgetComponent->SetVisibility(false);
}

void UInteractableBoxComponent::CheckValidInteractable(AGP3_MultiplayerCharacter* CharacterToCheckOver)
{
	if (!CharacterToCheckOver->TryReplaceInteractable(InteractableOwner)) return;

	//We are now the main interactable, yay!
	SetAsMainInteractable(CharacterToCheckOver);
	//We are now the main interactable, we don't need to check every frame anymore
	GetWorld()->GetTimerManager().ClearTimer(TimerCheckHandle);
}

void UInteractableBoxComponent::Handle_OnInteractableChanged(AGP3_MultiplayerCharacter* PlayerCharacter, TScriptInterface<IInteractable> NewInteractable)
{
	//If we are the new main interactable we already set ourself as the main one in CheckValidInteractable().
	if (NewInteractable.GetObject() == GetOwner()) return;

	//If we are not already checking to be the main interactable, we start now.
	//Also means this was the main interactable before this delegate was called.
	if (!GetWorld()->GetTimerManager().IsTimerActive(TimerCheckHandle))
	{
		//Begin to check if we can be the main interactable again.
		SetAsSideInteractable(PlayerCharacter);
	}
}
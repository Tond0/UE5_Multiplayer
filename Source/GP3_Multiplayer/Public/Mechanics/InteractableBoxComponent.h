// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Interactable.h"
#include "InteractableBoxComponent.generated.h"


/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GP3_MULTIPLAYER_API UInteractableBoxComponent : public UBoxComponent
{
	GENERATED_BODY()
	

public:
    UInteractableBoxComponent();

protected:
    UPROPERTY(BlueprintReadOnly, meta = (BlueprintProtected))
    TScriptInterface<IInteractable> InteractableOwner = nullptr;
public:
    UFUNCTION(BlueprintCallable, meta = (BluprintProtected))
    const TScriptInterface<IInteractable> GetInteractable() const { return InteractableOwner; }

public:
    /// <summary>
    /// Called from the Character for start an interaction.
    /// This function will broadcast OnInteracted, ik it might sounds stupid to make it not accessible for broadcast but then 
    /// place a public function to do so, but i think this way is much cleaner.
    /// </summary>
    UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
    void ExecuteInteract();
    void ExecuteInteract_Implementation() { InteractableOwner->Interact(); }

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);    

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Interactable.h"
#include "InteractableBoxComponent.generated.h"

class UWidgetComponent;

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
    TScriptInterface<IExecutable> InteractableOwner = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (BlueprintProtected))
    UWidgetComponent* InteractableWidgetComponent;

    FDelegateHandle HandleOnInteractableChanged;

public:
    UFUNCTION(BlueprintCallable, meta = (BluprintProtected))
    const TScriptInterface<IExecutable> GetInteractable() const { return InteractableOwner; }
    

//Check Variable
protected:
    /// <summary>
    /// Timer handle that will handle to check each CheckRate seconds, if the player is in range, if this is the closest interactable to him.
    /// </summary>
    FTimerHandle TimerCheckHandle;
    /// <summary>
    /// The rate which the boxcomponent will try to ask the main interactable to the player.
    /// </summary>
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BlueprintProtected))
    float CheckRateSeconds = 0.1f;
    /// <summary>
    /// Will check if it can be the main interactable of the player
    /// Called from the timer set in the OnOverlapBegin
    /// </summary>
    UFUNCTION()
    void CheckValidInteractable(AGP3_MultiplayerCharacter* CharacterToCheckOver);

    /// <summary>
    /// Set the interactable as the one the player could be interacting with in this moment.
    /// </summary>
    UFUNCTION()
    void SetAsMainInteractable(AGP3_MultiplayerCharacter* CharacterOverlapping);
    /// <summary>
    /// Set the interactable as the one the player can't interact with even if in range.
    /// This will start checking whenever this interactable is the closer to the player.
    /// </summary>
    /// <param name="CharacterOverlapping">The player we want to check</param>
    UFUNCTION()
    void SetAsSideInteractable(AGP3_MultiplayerCharacter* CharacterOverlapping);
    /// <summary>
    /// Set the interactale as the one the player can't interact with because it's not in range.
    /// </summary>
    UFUNCTION()
    void SetAsSleepInteractable(AGP3_MultiplayerCharacter* CharacterOverlapping);


public:
    /// <summary>
    /// Called from the Character for start an interaction.
    /// This function will broadcast OnInteracted, ik it might sounds stupid to make it not accessible for broadcast but then 
    /// place a public function to do so, but i think this way is much cleaner.
    /// </summary>
    UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
    void ExecuteInteract();
    void ExecuteInteract_Implementation() { InteractableOwner->Execute(); }

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UFUNCTION()
    void Handle_OnInteractableChanged(AGP3_MultiplayerCharacter* PlayerCharacter, TScriptInterface<IExecutable> NewInteractable);

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "InteractableBoxComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteracted);

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
    UPROPERTY(BlueprintAssignable)
    FOnInteracted OnInteracted;
public:
    /// <summary>
    /// Function to bind to OnInteracted, without giving the access to broadcast it.
    /// </summary>
    /// <typeparam name="UserClass"></typeparam>
    /// <param name="Owner">The owner of the funciton</param>
    /// <param name="Func">Usually in the form of &Class::Function</param>
    template <typename UserClass>
    void BindToOnInteracted(UserClass* Owner, void (UserClass::* Func)());

    /// <summary>
    /// Called from the Character for start an interaction.
    /// This function will broadcast OnInteracted, ik it might sounds stupid to make it not accessible for broadcast but then 
    /// place a public function to do so, but i think this way is much cleaner.
    /// </summary>
    UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
    void ExecuteInteract();
    void ExecuteInteract_Implementation() { OnInteracted.Broadcast(); }

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);    

};

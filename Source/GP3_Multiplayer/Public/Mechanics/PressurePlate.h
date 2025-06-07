// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Mechanics/Executable.h"
#include "PressurePlate.generated.h"

class UBoxComponent;

/**
 * 
 */
UCLASS(BlueprintType)
class GP3_MULTIPLAYER_API APressurePlate : public AExecutable
{
	GENERATED_BODY()
	
//Components
protected:
	APressurePlate();

	void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, meta = (BlueprintProtected))
	UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, meta = (BlueprintProtected))
	UStaticMeshComponent* MeshComponent;

	UFUNCTION(BlueprintCallable)
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void Handle_OnSizeChanged(EPowerState NewSizeState);

	UFUNCTION(BlueprintCallable)
	void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

//Behaviour
protected:
	/// <summary>
	/// If true this pressure plate will execute only if there's a player with the size state set to Big.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BlueprintProtected))
	bool NeedsBigPlayer;
};

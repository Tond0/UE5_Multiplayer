// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Components/TimelineComponent.h"
#include "Executable.h"
#include "Door.generated.h"

UCLASS()
class GP3_MULTIPLAYER_API ADoor : public AExecutable
{
	GENERATED_BODY()

//Components
protected:
	//Unreal automatically snaps the mesh as root if i dont place something else.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BlueprintProtected))
	USceneComponent* RootSceneComponent;
	//Parent of the mesh so we can adjust the pivot easily
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BlueprintProtected))
	USceneComponent* MeshContainerSceneComponent;
	//The door mesh
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BlueprintProtected))
	UStaticMeshComponent* MeshComponent;

//Timeline
protected:
	FTimeline DoorTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BlueprintProtected))
	UCurveFloat* CurveTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BlueprintProtected))
	float AnimationDuration;

	UPROPERTY(EditAnywhere, meta = (MakeEditWidget, BlueprintProtected))
	FVector StartRelativeLocation;
	
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget, BlueprintProtected))
	FVector TargetRelativeLocation = FVector(0, 100, 0);

	//It can be overriten in blueprint to add new funcitonality
	UFUNCTION(BlueprintNativeEvent)
	void Handle_TimelineUpdate(float Alpha);

	//It can be overriten in blueprint to add new funcitonality
	UFUNCTION(BlueprintNativeEvent)
	void Handle_OnTimelineFinished();

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BlueprintProtected))
	bool IsDoorOpen;
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected))
	const bool GetIsDoorOpen() const { return IsDoorOpen; }

public:	
	// Sets default values for this actor's properties
	ADoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Execute() override;


};

// Fill out your copyright notice in the Description page of Project Settings.

#include "Mechanics/Door.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootSceneComponent);

	//Created so we can adjust the pivot on the fly.
	MeshContainerSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MeshContainer"));
	MeshContainerSceneComponent->SetupAttachment(RootSceneComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(MeshContainerSceneComponent);
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
	//FIXME: Don't need this anymore
	////Transform to world space
	//StartLocation = UKismetMathLibrary::TransformLocation(GetActorTransform(), StartRelativeLocation);
	//TargetLocation = UKismetMathLibrary::TransformLocation(GetActorTransform(), TargetRelativeLocation);

	//Timeline Setup
	FOnTimelineFloat ProgressUpdate;
	ProgressUpdate.BindUFunction(this, FName("Handle_TimelineUpdate"));

	FOnTimelineEvent FinishedEvent;
	FinishedEvent.BindUFunction(this, FName("Handle_OnTimelineFinished"));

	DoorTimeline.SetPlayRate(1 / AnimationDuration);
	DoorTimeline.AddInterpFloat(CurveTimeline, ProgressUpdate);
	DoorTimeline.SetTimelineFinishedFunc(FinishedEvent);
}

void ADoor::Handle_TimelineUpdate_Implementation(float Alpha)
{
	FVector NewRelativeLocation = FMath::Lerp(StartRelativeLocation, TargetRelativeLocation, Alpha);
	MeshContainerSceneComponent->SetRelativeLocation(NewRelativeLocation);
}

void ADoor::Handle_OnTimelineFinished_Implementation()
{
	FVector FinalLocation = IsDoorOpen ? StartRelativeLocation : TargetRelativeLocation;
	
	MeshContainerSceneComponent->SetRelativeLocation(FinalLocation);

	IsDoorOpen = !IsDoorOpen;
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DoorTimeline.TickTimeline(DeltaTime);
}

void ADoor::Execute()
{
	Super::Execute();

	//Start door's animation
	if(IsDoorOpen)
		DoorTimeline.Reverse();
	else
		DoorTimeline.Play();

}

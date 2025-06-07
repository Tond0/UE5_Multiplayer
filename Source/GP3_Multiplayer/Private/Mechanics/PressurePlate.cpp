// Fill out your copyright notice in the Description page of Project Settings.


#include "Mechanics/PressurePlate.h"
#include "GP3_Multiplayer/GP3_MultiplayerCharacter.h"
#include "Mechanics/SizeChangerComponent.h"
#include "Components/BoxComponent.h"

APressurePlate::APressurePlate()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(FName("Box Component"));
	BoxComponent->SetupAttachment(MeshComponent);
}

void APressurePlate::BeginPlay()
{
	BoxComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &APressurePlate::OnBoxBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &APressurePlate::OnBoxEndOverlap);
}

void APressurePlate::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AGP3_MultiplayerCharacter* PlayerOverlapping = Cast<AGP3_MultiplayerCharacter>(OtherActor);
	if (!PlayerOverlapping) return;

	USizeChangerComponent* SizeChangerComponent = PlayerOverlapping->GetComponentByClass<USizeChangerComponent>();
	if (!SizeChangerComponent) return;

	//Does this need a big player?
	if (NeedsBigPlayer)
	{
		//We are interest only in the one player with the ability to grow.
		if (SizeChangerComponent->TargetPowerState != EPowerState::Big) return;

		//Is the player big?
		if (SizeChangerComponent->GetCurrentPowerState() == EPowerState::Big)
			Execute();

		//We need to know when the player change size on the PressurePlate
		//FIXME: Si sono abbastanza sicuro esista un OnComponentOverlap che funzioni come tick finché overlappa con qualcosa, ma con i delegate mi sembra più pulito e scalabile
		SizeChangerComponent->OnSizeChanged.AddUniqueDynamic(this, &APressurePlate::Handle_OnSizeChanged);
	}
	else
		Execute();
}

void APressurePlate::Handle_OnSizeChanged(EPowerState NewSizeState)
{
	//How did u even become small if we are listening to only the player able to grow
	if (NewSizeState == EPowerState::Small) return;
	
	Execute();
}

void APressurePlate::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AGP3_MultiplayerCharacter* PlayerOverlapping = Cast<AGP3_MultiplayerCharacter>(OtherActor);
	if (!PlayerOverlapping) return;

	USizeChangerComponent* SizeChangerComponent = PlayerOverlapping->GetComponentByClass<USizeChangerComponent>();
	if (!SizeChangerComponent) return;

	if (NeedsBigPlayer)
	{
		//We are interest only in the one player with the ability to grow.
		if (SizeChangerComponent->TargetPowerState != EPowerState::Big) return;

		//Is the player big?
		if (SizeChangerComponent->GetCurrentPowerState() == EPowerState::Big)
			Execute();

		//We're not interest to the delegate anymore.
		SizeChangerComponent->OnSizeChanged.RemoveDynamic(this, &APressurePlate::Handle_OnSizeChanged);
	}
	else
		Execute();
	
}
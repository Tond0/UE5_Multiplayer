// Copyright Epic Games, Inc. All Rights Reserved.

#include "GP3_MultiplayerGameMode.h"
#include "GP3_MultiplayerCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Public/Mechanics/SizeChangerComponent.h"
#include "Kismet/GameplayStatics.h"

void AGP3_MultiplayerGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	ACharacter* NewCharacter = NewPlayer->GetCharacter();
	AGP3_MultiplayerCharacter* NewProjectCharacter = Cast<AGP3_MultiplayerCharacter>(NewCharacter);
	const USizeChangerComponent* SizeChangerComponent =	NewProjectCharacter->GetSizeChangerComponent();

	if (NewPlayer == UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		NewProjectCharacter->SetTargetPowerState(EPowerState::Small);
	}
	else if(NewPlayer == UGameplayStatics::GetPlayerController(GetWorld(), 1))
	{
		NewProjectCharacter->SetTargetPowerState(EPowerState::Big);
	}
}

AGP3_MultiplayerGameMode::AGP3_MultiplayerGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

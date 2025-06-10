// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GP3_MultiplayerCharacter.h"
#include "GP3_MultiplayerGameMode.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStart, AGP3_MultiplayerCharacter*, PlayerSpawned);

UCLASS(minimalapi)
class AGP3_MultiplayerGameMode : public AGameModeBase
{
	GENERATED_BODY()


public:
	UPROPERTY(BlueprintAssignable)
	FOnPlayerStart OnPlayerStart;

public:
	AGP3_MultiplayerGameMode();

	void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
};




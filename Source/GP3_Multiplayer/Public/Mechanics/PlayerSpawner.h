// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerSpawner.generated.h"

UCLASS()
class GP3_MULTIPLAYER_API APlayerSpawner : public AActor
{
	GENERATED_BODY()
	
protected:
	/// <summary>
	/// Quale player deve spawnare qua? player 1? player 2?
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BlueprintProtected, ClampMin = "1", ClampMax = "2"))
	int PlayerIndex = 1;

public:	
	// Sets default values for this actor's properties
	APlayerSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Handle_OnPlayerStart(AGP3_MultiplayerCharacter* Character);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};

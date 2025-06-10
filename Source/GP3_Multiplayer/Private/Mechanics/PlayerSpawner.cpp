// Fill out your copyright notice in the Description page of Project Settings.


#include "Mechanics/PlayerSpawner.h"
#include "Kismet/GameplayStatics.h"
#include "GP3_Multiplayer/GP3_MultiplayerGameMode.h"
#include "GP3_Multiplayer/GP3_MultiplayerCharacter.h"

// Sets default values
APlayerSpawner::APlayerSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	if (!HasAuthority()) return;

	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(GetWorld());
	AGP3_MultiplayerGameMode* ProjectGameMode = Cast<AGP3_MultiplayerGameMode>(GameModeBase);
	if (!ProjectGameMode) return;

	ProjectGameMode->OnPlayerStart.AddUniqueDynamic(this, &APlayerSpawner::Handle_OnPlayerStart);
}

// Called every frame
void APlayerSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//FIXME: Questa funzione funziona solo su i client, per l'host ho semplicamente piazzato il player start dove si trova lo spawner del giocatore 0.
void APlayerSpawner::Handle_OnPlayerStart_Implementation(AGP3_MultiplayerCharacter* Character)
{
	if (!Character) return;
	//This is not the player we want to set.
	if (Character->GetController() != UGameplayStatics::GetPlayerController(GetWorld(), PlayerIndex-1)) return;
	//We move the character to the spawn location
	Character->SetActorLocation(GetActorLocation());
}

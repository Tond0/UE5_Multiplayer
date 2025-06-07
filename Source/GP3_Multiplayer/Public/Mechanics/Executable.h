// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Executable.generated.h"

/// <summary>
/// ExecutableInvoker potrebbe benissimo essere una leva, un bottone, una pressure plate,...
/// </summary>
UCLASS(Abstract, Blueprintable, BlueprintType	)
class GP3_MULTIPLAYER_API AExecutable : public AActor, public IInteractable
{
	GENERATED_BODY()
	
//Can Invoke
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BlueprintProtected))
	bool CanExecute = true;

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected))
	void DisableExecutable();

	void Interact_Implementation() override;

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected))
	const bool GetCanInvoke() const { return CanExecute; }

//Editor Behaviour Property
protected:
	/// <summary>
	/// If true this invoker will fire the invoke only once.
	/// If false this will act more like a toggle.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BlueprintProtected))
	bool ExecuteOnce = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BlueprintProtected))
	TArray<AExecutable*> TargetExecutables;

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected))
	void ExecuteOnTargets();

public:
	UFUNCTION(BlueprintCallable)
	virtual void Execute();

public:	
	// Sets default values for this actor's properties
	AExecutable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};

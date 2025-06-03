// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SizeChangerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP3_MULTIPLAYER_API USizeChangerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USizeChangerComponent();

protected:
	/// <summary>
	/// The size the character will reach if the power-up make it small
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BlueprintProtected))
	FVector SmallSize;

	/// <summary>
	/// The size the character will reach if the power-up make it big
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BlueprintProtected))
	FVector BigSize;

	/// <summary>
	/// Is this character in its normal size or it's altering its size?
	/// </summary>
	UPROPERTY(Replicated, BlueprintReadOnly, meta = (BlueprintProtected))
	bool isAlteringSize;

public:
	/// <summary>
	/// The size this character will grow/shrink to.
	/// </summary>
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	FVector TargetSize;

//Getters
public:
	UFUNCTION(BlueprintCallable)
	const bool GetIsAleteringSize() const { return isAlteringSize; }

	UFUNCTION(BlueprintCallable)
	const FVector GetSmallSize() const { return SmallSize; }

	UFUNCTION(BlueprintCallable)
	const FVector GetBigSize() const { return BigSize; }

protected:
	/// <summary>
	/// This will be called each time the player perform the X action.
	/// </summary>
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, meta = (BlueprintProtected))
	void ToggleSize();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};

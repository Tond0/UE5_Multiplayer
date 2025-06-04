// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SizeChangerComponent.generated.h"


UENUM(BlueprintType)
enum class EPowerState
{
	Standard,
	Big,
	Small
};

USTRUCT(BlueprintType)
struct FStatePowerSettings
{
	GENERATED_BODY()

public:

	FStatePowerSettings() {}

	FStatePowerSettings(FVector Size, float JumpZVelocity, float GravityScale, float MaxWalkSpeed)
	{
		this->Size = Size;
		this->JumpZVelocity = JumpZVelocity;
		this->GravityScale = GravityScale;
		this->MaxWalkSpeed = MaxWalkSpeed;
	}

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector Size;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float JumpZVelocity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float GravityScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxWalkSpeed;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP3_MULTIPLAYER_API USizeChangerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USizeChangerComponent();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BlueprintProtected))
	FStatePowerSettings BigPowerSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BlueprintProtected))
	FStatePowerSettings SmallPowerSettings;
	UPROPERTY(BlueprintReadOnly, meta = (BlueprintProtected))
	FStatePowerSettings StandardPowerSettings;

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected))
	void ApplyNewSettings(AActor* ActorOwner, FStatePowerSettings SettingsToApply);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BlueprintProtected))
	class UCharacterMovementComponent* MovementComponent;

	UPROPERTY(BlueprintReadOnly, meta = (BlueprintProtected))
	float StandardJumpZVelocity;

protected:
	/// <summary>
	/// In which state is this character in?
	/// </summary>
	UPROPERTY(Replicated, BlueprintReadOnly, meta = (BlueprintProtected))
	EPowerState CurrentPowerState;

public:
	/// <summary>
	/// When the action will be triggered will this character try to Grow or Shrink?
	/// </summary>
	UPROPERTY(BlueprintReadOnly, meta = (BlueprintProtected))
	EPowerState TargetPowerState;

//Getters
public:
	UFUNCTION(BlueprintCallable)
	const EPowerState GetPowerState() const { return CurrentPowerState; }

protected:
	/// <summary>
	/// Change the power state and adjust the character with it.
	/// </summary>
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, meta = (BlueprintProtected))
	void ChangePowerState(EPowerState NextPowerState);

	/// <summary>
	/// Toggle from the standard state and the target state.
	/// Called from power action delegate.
	/// </summary>
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected))
	void TogglePowerState();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};

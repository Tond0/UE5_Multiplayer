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
	FVector Size = FVector(0);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float JumpZVelocity = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float GravityScale = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxWalkSpeed = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSizeChanged, EPowerState, NewSizeState);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP3_MULTIPLAYER_API USizeChangerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USizeChangerComponent();

	UFUNCTION(NetMulticast, Reliable)
	void SetUp(AGP3_MultiplayerCharacter* Character);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BlueprintProtected))
	FStatePowerSettings BigPowerSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BlueprintProtected))
	FStatePowerSettings SmallPowerSettings;
	UPROPERTY(BlueprintReadOnly, meta = (BlueprintProtected))
	FStatePowerSettings StandardPowerSettings;

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected))
	void ApplyNewSettings(AActor* ActorOwner, FStatePowerSettings SettingsToApply);

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected))
	bool CheckIfScalable(FVector SizeToCheck);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BlueprintProtected))
	class UCharacterMovementComponent* MovementComponent;

	UPROPERTY(BlueprintReadOnly, meta = (BlueprintProtected))
	class UCapsuleComponent* CapsuleComponent;

	UPROPERTY(BlueprintReadOnly, meta = (BlueprintProtected))
	float StandardJumpZVelocity;

//Delegate
//FIXME: Grazie ad Unreal sono obbligato a lasciare questo delegate public se voglio che possa essere riprodotto in BP (Dynamic).
//		 Questo perché creare una funzione per il binding come ho fatto con il resto dei delegate è stupidamente complesso 
//		 e non ho trovato online nessuna risorsa al riguardo, pure chatgpt mi ha dato forfeit. Quando avrò più tempo ci darò occhio.
public:
	UPROPERTY(BlueprintAssignable)
	FOnSizeChanged OnSizeChanged;


protected:
	/// <summary>
	/// In which state is this character in?
	/// </summary>
	UPROPERTY(ReplicatedUsing = OnRep_CurrentPowerState, BlueprintReadOnly, meta = (BlueprintProtected))
	EPowerState CurrentPowerState = EPowerState::Standard;

	UFUNCTION()
	void OnRep_CurrentPowerState();

public:
	/// <summary>
	/// When the action will be triggered will this character try to Grow or Shrink?
	/// </summary>
	UPROPERTY(Replicated, BlueprintReadOnly, meta = (BlueprintProtected))
	EPowerState TargetPowerState;

//Getters
public:
	UFUNCTION(BlueprintCallable)
	const EPowerState GetCurrentPowerState() const { return CurrentPowerState; }

protected:
	/// <summary>
	/// Change the power state and adjust the character with it.
	/// </summary>
	UFUNCTION(Server, Reliable, BlueprintCallable, meta = (BlueprintProtected))
	void ChangePowerState(EPowerState NextPowerState);

	/// <summary>
	/// Toggle from the standard state and the target state.
	/// Called from power action delegate.
	/// </summary>
	UFUNCTION(Server, Reliable, BlueprintCallable, meta = (BlueprintProtected))
	void TogglePowerState();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};

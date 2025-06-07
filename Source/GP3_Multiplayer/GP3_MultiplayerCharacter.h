// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "GP3_MultiplayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class USizeChangerComponent;
class UInteractableBoxComponent;

struct FInputActionValue;

/// <summary>
/// We don't need this to be dynamic.
/// </summary>
/// <param name=""></param>
DECLARE_DELEGATE(FOnPowerActionPerformed);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInteractableChanged, AGP3_MultiplayerCharacter*, TScriptInterface<IInteractable>);

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AGP3_MultiplayerCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USizeChangerComponent* SizeComponent;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Power Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PowerAction;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

public:
	AGP3_MultiplayerCharacter();


//Delegates
//PowerAction
protected:
	FOnPowerActionPerformed OnPowerActionPerformed;
public:
	bool BindToOnPowerActionPerformed(UObject* Owner, FName FunctionName);
	
//InteractableChanged
protected:
	FOnInteractableChanged OnInteractableChanged;
public:
	FDelegateHandle BindToOnInteractableChanged(UObject* Object, FName FunctionName);
	void UnbindToOnInteractableChanged(FDelegateHandle Handle);
	
//Interactable
protected:
	/// <summary>
	/// The interactable we can interact with.
	/// Self assigned from the interactable box if player is close to it.
	/// </summary>
	UPROPERTY(BlueprintReadOnly)
	TScriptInterface<IInteractable> Interactable;
	UFUNCTION(Server, Reliable)
	void Server_RequestInteract(const FInputActionValue& Value);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_Interact();
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected))
	const TScriptInterface<IInteractable> GetInteractable() const { return Interactable; }

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected))
	bool TryReplaceInteractable(TScriptInterface<IInteractable> NewInteractable);

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected))
	void RemoveInteractable(TScriptInterface<IInteractable> InteractableToRemove);


protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void Server_PerfomPower(const FInputActionValue& Value);
	
public:
	UFUNCTION(BlueprintCallable)
	const USizeChangerComponent* GetSizeChangerComponent() const { return SizeComponent; }

	//FIXME: Multicast o Client?
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetTargetPowerState(EPowerState NewTargetPowerState);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

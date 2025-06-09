// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

/**
 * 
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};


class GP3_MULTIPLAYER_API IInteractable
{
    GENERATED_BODY()

public:
    // Puoi chiamare questa funzione in C++ o Blueprints
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void Interact();

    //FIXME: Deprecated
    /*UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    bool GetIsExecutable() const;

protected:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void SetIsExecutable(bool IsExecutable);*/
};
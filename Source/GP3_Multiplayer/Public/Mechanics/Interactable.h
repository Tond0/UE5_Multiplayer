// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

/**
 * 
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UExecutable : public UInterface
{
	GENERATED_BODY()
};


class GP3_MULTIPLAYER_API IExecutable
{
    GENERATED_BODY()

public:
    // Puoi chiamare questa funzione in C++ o Blueprints
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void Execute();
};
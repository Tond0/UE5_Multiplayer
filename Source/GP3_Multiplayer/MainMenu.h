// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "MainMenu.generated.h"

USTRUCT()
struct FServerData
{
	GENERATED_BODY()

	FString Name;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
	FString HostUsername;
};


/**
 * 
 */
UCLASS()
class GP3_MULTIPLAYER_API UMainMenu : public UMenuWidget
{
	GENERATED_BODY()
	
public:
	UMainMenu(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	void SetServerList(TArray<FServerData> ServerData);

	void SelectIndex(uint32 Index);

protected:
	
	virtual bool Initialize();

private:

	TOptional<uint32> SelectedIndex;

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu;

	UPROPERTY(meta = (BindWidget))
	class UWidget* HostMenu;

	UPROPERTY(meta = (BindWidget))
	class UWidget* JoinMenu;

	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;

	UPROPERTY(meta=(BindWidget))
	class UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
	class UEditableText* ServerHostName;

	UPROPERTY(meta = (BindWidget))
	class UButton* CancelHostMenuButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* ConfirmHostMenuButton;

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ServerList;

	TSubclassOf<class UUserWidget> ServerRowClass;

	UPROPERTY(meta = (BindWidget))
	class UButton* ConfirmJoinMenuButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* CancelJoinMenuButton;

	

	UFUNCTION()
	void HostServer();

	UFUNCTION()
	void JoinServer();

	UFUNCTION()
	void QuitPressed();

	UFUNCTION()
	void OpenMainMenu();

	UFUNCTION()
	void OpenHostMenu();

	UFUNCTION()
	void OpenJoinMenu();

	UFUNCTION()
	void UpdateChildren();
};

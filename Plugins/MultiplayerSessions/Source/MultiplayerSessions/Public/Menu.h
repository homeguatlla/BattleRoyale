// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup();

protected:
	virtual bool Initialize() override;
	
private:
	void SetInputMode();
	void SaveMultiplayerSessionsSubsystem();
	
	UFUNCTION()
	void HostButtonClicked();
	UFUNCTION()
	void JoinButtonClicked();
	
	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton; //the exact same name than the button name in the widget
	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	//The subsystem designed to handle all online session functionality
	class UMultiplayerSessionsSubsystem* mMultiplayerSessionsSubsystem;
};

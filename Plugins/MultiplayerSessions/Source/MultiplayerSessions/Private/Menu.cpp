// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"

#include "MultiplayerSessionsSubsystem.h"
#include "Components/Button.h"

void UMenu::MenuSetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	SetInputMode();
	SaveMultiplayerSessionsSubsystem();
}

bool UMenu::Initialize()
{
	if(!Super::Initialize())
	{
		return false;
	}

	if(HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}
	if(JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}
	
	return true;
}

void UMenu::SetInputMode()
{
	const auto world = GetWorld();
	if(!world)
	{
		return;
	}
	const auto playerController = world->GetFirstPlayerController();
	if(!playerController)
	{
		return;
	}

	FInputModeUIOnly inputModeData;
	inputModeData.SetWidgetToFocus(TakeWidget());
	inputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	
	playerController->SetInputMode(inputModeData);
	playerController->SetShowMouseCursor(true);
}

void UMenu::SaveMultiplayerSessionsSubsystem()
{
	const auto gameInstance = GetGameInstance();
	if(gameInstance)
	{
		mMultiplayerSessionsSubsystem = gameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}
}

void UMenu::HostButtonClicked()
{
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage( -1, 15.0f, FColor::Yellow, FString("Host button clicked"));
	}

	if(!mMultiplayerSessionsSubsystem)
	{
		return;
	}
	
	mMultiplayerSessionsSubsystem->CreateSession(4, FString("FreeForAll"));
}

void UMenu::JoinButtonClicked()
{
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage( -1, 15.0f, FColor::Yellow, FString("Join button clicked"));
	}
}

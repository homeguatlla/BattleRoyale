// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"

#include "MultiplayerSessionsSubsystem.h"
#include "Components/Button.h"

void UMenu::Setup(int numberPublicConnections, const FString& matchType)
{
	mNumPublicConnections = numberPublicConnections;
	mMatchType = matchType;

	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	SetInputMode();
	SaveMultiplayerSessionsSubsystem();

	SubscribeToMultiplayerSessionDelegates();
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

void UMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	TearDown();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}

void UMenu::OnCreateSession(bool wasSuccessful)
{
	if(!wasSuccessful)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.0f,
			FColor::Red,
			FString(TEXT("UMenu::OnCreateSession fail to create session!")));
		return;
	}
	
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.0f,
			FColor::Yellow,
			FString(TEXT("UMenu::OnCreateSession successfully!")));
	}

	const auto world = GetWorld();
	if(!world)
	{
		return;
	}
	//TODO replace this by an access to the game mode
	world->ServerTravel("/Game/Maps/TestMultiplayerSessions/TestMultiplayerSessionsLobby?listen");
}

void UMenu::HostButtonClicked()
{
	if(!mMultiplayerSessionsSubsystem)
	{
		return;
	}
	
	mMultiplayerSessionsSubsystem->CreateSession(mNumPublicConnections, mMatchType);
}

void UMenu::JoinButtonClicked()
{
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage( -1, 15.0f, FColor::Yellow, FString("Join button clicked"));
	}
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

void UMenu::TearDown()
{
	RemoveFromParent();
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
	FInputModeGameOnly inputModeData;
	playerController->SetInputMode(inputModeData);
	playerController->SetShowMouseCursor(false);
}

void UMenu::SubscribeToMultiplayerSessionDelegates()
{
	if(mMultiplayerSessionsSubsystem)
	{
		mMultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
	}
}
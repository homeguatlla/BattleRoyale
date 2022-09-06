// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"

#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "Components/Button.h"

void UMenu::Setup(int numberPublicConnections, const FString& matchType, const FString& lobbyPath)
{
	mNumPublicConnections = numberPublicConnections;
	mMatchType = matchType;
	mPathToLobby = FString::Printf(TEXT("%s?listen"), *lobbyPath);

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

	const auto world = GetWorld();
	if(!world)
	{
		return;
	}
	const auto gameMode = world->GetAuthGameMode();
	world->ServerTravel(mPathToLobby);
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& sessionsResults, bool wasSucccessful)
{
	if(mMultiplayerSessionsSubsystem == nullptr)
	{
		return;
	}
	
	for(auto&& result : sessionsResults)
	{
		FString settingsMatchTypeValue;
		result.Session.SessionSettings.Get(FName("MatchType"), settingsMatchTypeValue);
		if(settingsMatchTypeValue == mMatchType)
		{
			mMultiplayerSessionsSubsystem->JoinSession(result);
			return;
		}
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type result)
{
	const auto onlineSubsystem = IOnlineSubsystem::Get();
	if(!onlineSubsystem)
	{
		return;
	}
	
	const auto sessionInterface = onlineSubsystem->GetSessionInterface();
	if(!sessionInterface.IsValid())
	{
		return;
	}
	
	FString address;
	sessionInterface->GetResolvedConnectString(NAME_GameSession, address);
	const auto playerController = GetGameInstance()->GetFirstLocalPlayerController();
	if(playerController)
	{
		playerController->ClientTravel(address, ETravelType::TRAVEL_Absolute);
	}
}

void UMenu::OnDestroySession(bool wasSuccessful)
{
}

void UMenu::OnStartSession(bool wasSuccessful)
{
	
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
	if(mMultiplayerSessionsSubsystem)
	{
		mMultiplayerSessionsSubsystem->FindSessions(10000);
		
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
		mMultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		mMultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		mMultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		mMultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::UMenu::OnStartSession);
	}
}
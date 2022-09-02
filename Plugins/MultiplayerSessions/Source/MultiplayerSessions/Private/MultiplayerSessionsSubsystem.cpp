// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem() :
CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartOnlineGameComplete))
{
	const auto onlineSubsystem = IOnlineSubsystem::Get();
	if(onlineSubsystem)
	{
		mSessionInterface = onlineSubsystem->GetSessionInterface();
	}
}

void UMultiplayerSessionsSubsystem::CreateSession(int numPublicConnections, const FString& matchType)
{
	if(!mSessionInterface.IsValid())
	{
		return;
	}
	
	const auto existingSession = mSessionInterface->GetNamedSession(NAME_GameSession);
	if(existingSession != nullptr)
	{
		mSessionInterface->DestroySession(NAME_GameSession);
	}
	//store the delegate in a FDelegateHandle so we ccan later remove it from the delegate list
	mCreateSessionCompleteDelegateHandle = mSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	mLastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	mLastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	mLastSessionSettings->NumPublicConnections = numPublicConnections;
	mLastSessionSettings->bAllowJoinInProgress = true;
	mLastSessionSettings->bAllowJoinViaPresence = true;
	mLastSessionSettings->bShouldAdvertise = true;
	mLastSessionSettings->bUsesPresence = true;
	mLastSessionSettings->Set(
		FName("MatchType"),
		matchType,
		EOnlineDataAdvertisementType::ViaOnlineService);

	const auto localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if(!mSessionInterface->CreateSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *mLastSessionSettings))
	{
		mSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(mCreateSessionCompleteDelegateHandle);
		//Broadcast our own custom delegate
		MultiplayerOnCreateSessionComplete.Broadcast(false);
		UE_LOG(LogTemp, Error, TEXT("UMultiplayerSessionsSubsystem::CreateSession Error creating Game Session"));
	}
}

void UMultiplayerSessionsSubsystem::FindSessions(int maxSearchResults)
{
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& sessionsResult)
{
}

void UMultiplayerSessionsSubsystem::DestroySession()
{
}

void UMultiplayerSessionsSubsystem::StartSession()
{
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName sessionName, bool wasSuccessful)
{
	if(mSessionInterface)
	{
		mSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(mCreateSessionCompleteDelegateHandle);
	}
	
	MultiplayerOnCreateSessionComplete.Broadcast(wasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool wasSuccessful)
{
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName sessionName, bool wasSuccessful)
{
}

void UMultiplayerSessionsSubsystem::OnStartOnlineGameComplete(FName sessionName, bool wasSuccessful)
{
}

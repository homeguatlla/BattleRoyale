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
	mLastSessionSettings->BuildUniqueId = 1;
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
	if(!mSessionInterface.IsValid())
	{
		return;
	}
	
	mFindSessionCompleteDelegateHandle = mSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	mLastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	mLastSessionSearch->MaxSearchResults = maxSearchResults;
	mLastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	mLastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	
	const auto localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if(!mSessionInterface->FindSessions(*localPlayer->GetPreferredUniqueNetId(), mLastSessionSearch.ToSharedRef()))
	{
		mSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(mFindSessionCompleteDelegateHandle);
		MultiplayerOnFindSessionsComplete.Broadcast({}, false);
		UE_LOG(LogTemp, Error, TEXT("UMultiplayerSessionsSubsystem::FindSessions Error finding Game Sessions"));
	}
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& sessionsResult)
{
	if(!mSessionInterface.IsValid())
	{
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		UE_LOG(LogTemp, Error, TEXT("UMultiplayerSessionsSubsystem::JoinSession Error joining session"));
		return;
	}
	mJoinSessionCompleteDelegateHandle = mSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);
	const auto localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if(!mSessionInterface->JoinSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, sessionsResult))
	{
		mSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(mJoinSessionCompleteDelegateHandle);
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		UE_LOG(LogTemp, Error, TEXT("UMultiplayerSessionsSubsystem::JoinSession Error joining session"));
	}
}

void UMultiplayerSessionsSubsystem::DestroySession()
{
}

void UMultiplayerSessionsSubsystem::StartSession()
{
	if(!mSessionInterface.IsValid() || mLastSessionSearch->SearchResults.Num() <= 0)
	{
		MultiplayerOnStartSessionComplete.Broadcast(false);
		UE_LOG(LogTemp, Error, TEXT("UMultiplayerSessionsSubsystem::StartSession Error couldn't start session."));
		return;
	}

	mStartSessionCompleteDelegateHandle = mSessionInterface->AddOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegate);
	mSessionInterface->StartSession(NAME_GameSession);
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
	if(!mSessionInterface)
	{
		return;
	}
	
	mSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(mFindSessionCompleteDelegateHandle);
	
	//no sessions found
	if(mLastSessionSearch->SearchResults.Num() <= 0)
	{
		MultiplayerOnFindSessionsComplete.Broadcast({}, false);
	}
	else
	{
		MultiplayerOnFindSessionsComplete.Broadcast(mLastSessionSearch->SearchResults, wasSuccessful);
	}
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	if(!mSessionInterface)
	{
		return;
	}
	mSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(mJoinSessionCompleteDelegateHandle);
	MultiplayerOnJoinSessionComplete.Broadcast(result);
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName sessionName, bool wasSuccessful)
{
}

void UMultiplayerSessionsSubsystem::OnStartOnlineGameComplete(FName sessionName, bool wasSuccessful)
{
	if(!mSessionInterface)
	{
		return;
	}
	mSessionInterface->ClearOnStartSessionCompleteDelegate_Handle(mStartSessionCompleteDelegateHandle);
	MultiplayerOnStartSessionComplete.Broadcast(wasSuccessful);
}

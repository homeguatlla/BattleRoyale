// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem() :
CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartOnlineGameComplete))
{
}

IOnlineSessionPtr UMultiplayerSessionsSubsystem::GetSessionInterface() const
{
	const auto onlineSubsystem = Online::GetSubsystem(GetWorld());
	return onlineSubsystem ? onlineSubsystem->GetSessionInterface() : nullptr;
}

void UMultiplayerSessionsSubsystem::CreateSession(int numPublicConnections, const FString& matchType)
{
	const auto sessionInterface = GetSessionInterface();
	if(!sessionInterface.IsValid())
	{
		return;
	}

	const auto existingSession = sessionInterface->GetNamedSession(NAME_GameSession);
	if(existingSession != nullptr)
	{
		//if a session already exists we will destroy it. But destruction takes time so we can not
		//create the session immediately we need to wait the OnDestroySessionComplete delegate and
		//that is the reason we need to save the numPublicConnections and the matchType to variables.
		mShouldCreateSessionOnDestroy = true;
		mLastNumPublicConnections = numPublicConnections;
		mLastMatchType = matchType;
		DestroySession();
	}
	//store the delegate in a FDelegateHandle so we ccan later remove it from the delegate list
	mCreateSessionCompleteDelegateHandle = sessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	mLastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	mLastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	mLastSessionSettings->NumPublicConnections = numPublicConnections;
	mLastSessionSettings->bAllowJoinInProgress = true;
	mLastSessionSettings->bAllowJoinViaPresence = true;
	mLastSessionSettings->bShouldAdvertise = true;
	mLastSessionSettings->bUsesPresence = true;
	mLastSessionSettings->BuildUniqueId = 1;
	mLastSessionSettings->bUseLobbiesIfAvailable = true;
	mLastSessionSettings->Set(
		FName("MatchType"),
		matchType,
		EOnlineDataAdvertisementType::ViaOnlineService);

	const auto localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if(!sessionInterface->CreateSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *mLastSessionSettings))
	{
		sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(mCreateSessionCompleteDelegateHandle);
		//Broadcast our own custom delegate
		MultiplayerOnCreateSessionComplete.Broadcast(false);
		UE_LOG(LogTemp, Error, TEXT("UMultiplayerSessionsSubsystem::CreateSession Error creating Game Session"));
	}
}

void UMultiplayerSessionsSubsystem::FindSessions(int maxSearchResults)
{
	const auto sessionInterface = GetSessionInterface();
	if(!sessionInterface.IsValid())
	{
		return;
	}

	mFindSessionCompleteDelegateHandle = sessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	mLastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	mLastSessionSearch->MaxSearchResults = maxSearchResults;
	mLastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	mLastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const auto localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if(!sessionInterface->FindSessions(*localPlayer->GetPreferredUniqueNetId(), mLastSessionSearch.ToSharedRef()))
	{
		sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(mFindSessionCompleteDelegateHandle);
		MultiplayerOnFindSessionsComplete.Broadcast({}, false);
		UE_LOG(LogTemp, Error, TEXT("UMultiplayerSessionsSubsystem::FindSessions Error finding Game Sessions"));
	}
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& sessionsResult)
{
	const auto sessionInterface = GetSessionInterface();
	if(!sessionInterface.IsValid())
	{
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		UE_LOG(LogTemp, Error, TEXT("UMultiplayerSessionsSubsystem::JoinSession Error joining session"));
		return;
	}
	mJoinSessionCompleteDelegateHandle = sessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);
	const auto localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if(!sessionInterface->JoinSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, sessionsResult))
	{
		sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(mJoinSessionCompleteDelegateHandle);
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		UE_LOG(LogTemp, Error, TEXT("UMultiplayerSessionsSubsystem::JoinSession Error joining session"));
	}
}

void UMultiplayerSessionsSubsystem::DestroySession()
{
	const auto sessionInterface = GetSessionInterface();
	if(!sessionInterface.IsValid())
	{
		MultiplayerOnDestroySessionComplete.Broadcast(false);
		return;
	}
	mDestroySessionCompleteDelegateHandle = sessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);
	if(!sessionInterface->DestroySession(NAME_GameSession))
	{
		sessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(mDestroySessionCompleteDelegateHandle);
		MultiplayerOnDestroySessionComplete.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::StartSession()
{
	const auto sessionInterface = GetSessionInterface();
	if(!sessionInterface.IsValid() || mLastSessionSearch->SearchResults.Num() <= 0)
	{
		MultiplayerOnStartSessionComplete.Broadcast(false);
		UE_LOG(LogTemp, Error, TEXT("UMultiplayerSessionsSubsystem::StartSession Error couldn't start session."));
		return;
	}

	mStartSessionCompleteDelegateHandle = sessionInterface->AddOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegate);
	if(!sessionInterface->StartSession(NAME_GameSession))
	{
		sessionInterface->ClearOnStartSessionCompleteDelegate_Handle(mStartSessionCompleteDelegateHandle);
		MultiplayerOnStartSessionComplete.Broadcast(false);
	}
}

EOnlineAsyncTaskState::Type UMultiplayerSessionsSubsystem::GetFindSessionsStatus() const
{
	if(mLastSessionSearch)
	{
		return mLastSessionSearch->SearchState;
	}
	
	return EOnlineAsyncTaskState::NotStarted;
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName sessionName, bool wasSuccessful)
{
	if(const auto sessionInterface = GetSessionInterface())
	{
		sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(mCreateSessionCompleteDelegateHandle);
	}

	MultiplayerOnCreateSessionComplete.Broadcast(wasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool wasSuccessful)
{
	const auto sessionInterface = GetSessionInterface();
	if(!sessionInterface)
	{
		return;
	}

	sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(mFindSessionCompleteDelegateHandle);

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
	const auto sessionInterface = GetSessionInterface();
	if(!sessionInterface)
	{
		return;
	}
	sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(mJoinSessionCompleteDelegateHandle);
	MultiplayerOnJoinSessionComplete.Broadcast(result);
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName sessionName, bool wasSuccessful)
{
	const auto sessionInterface = GetSessionInterface();
	if(!sessionInterface)
	{
		return;
	}

	sessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(mDestroySessionCompleteDelegateHandle);
	//In case a previous session was created and we destroy it, we can create the new session
	if(wasSuccessful && mShouldCreateSessionOnDestroy)
	{
		mShouldCreateSessionOnDestroy = false;
	}
	MultiplayerOnDestroySessionComplete.Broadcast(wasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnStartOnlineGameComplete(FName sessionName, bool wasSuccessful)
{
	const auto sessionInterface = GetSessionInterface();
	if(!sessionInterface)
	{
		return;
	}
	sessionInterface->ClearOnStartSessionCompleteDelegate_Handle(mStartSessionCompleteDelegateHandle);
	MultiplayerOnStartSessionComplete.Broadcast(wasSuccessful);
}

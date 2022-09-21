// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MultiplayerSessionsSubsystem.generated.h"

//Declaring our own custom delegates for the Menu class to bind callbacks to
//DYNAMIC_MULTICAST_DELEGATE can be used from blueprints
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnCreateSessionComplete, bool, wasSuccessful);
//In this case, as FOnlineSessionsSearchResult is not a blueprintable type (not uclass, ustruct), we need to change the delegate type and
//using MULTICAST_DELEGATE will not be possible to call it from blueprints. The way to fix this is creating a struct and
//pass the information into it.
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& sessionsResults, bool wasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnDestroySessionComplete, bool, wasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnStartSessionComplete, bool, wasSuccessful);

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMultiplayerSessionsSubsystem();

	//Go handle session functionality. The Menu class will call these
	void CreateSession(int numPublicConnections, const FString& matchType);
	void FindSessions(int maxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& sessionsResult);
	void DestroySession();
	void StartSession();
	EOnlineAsyncTaskState::Type GetFindSessionsStatus() const;
	
	//Our own custom delegates for the Menu class to bind callbacks to
	FMultiplayerOnCreateSessionComplete MultiplayerOnCreateSessionComplete;
	FMultiplayerOnFindSessionsComplete MultiplayerOnFindSessionsComplete;
	FMultiplayerOnJoinSessionComplete MultiplayerOnJoinSessionComplete;
	FMultiplayerOnDestroySessionComplete MultiplayerOnDestroySessionComplete;
	FMultiplayerOnStartSessionComplete MultiplayerOnStartSessionComplete;
	
protected:
	//Internal callbacks for the delegates we'll add to the Online Session Interface delegate list.
	//These don't need to be called this class.
	void OnCreateSessionComplete(FName sessionName, bool wasSuccessful);
	void OnFindSessionsComplete(bool wasSuccessful);
	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);
	void OnDestroySessionComplete(FName sessionName, bool wasSuccessful);
	void OnStartOnlineGameComplete(FName sessionName, bool wasSuccessful);
	
private:
	IOnlineSessionPtr mSessionInterface;
	TSharedPtr<FOnlineSessionSettings> mLastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> mLastSessionSearch;
	bool mShouldCreateSessionOnDestroy{false};
	int mLastNumPublicConnections;
	FString mLastMatchType;

	//To add to the Online Session Interface delegate list.
	//We'll bind our MultiplayerSessionsSubsystem interal callbacks to these.
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle mCreateSessionCompleteDelegateHandle;
	
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle mFindSessionCompleteDelegateHandle;
	
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle mJoinSessionCompleteDelegateHandle;
	
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle mDestroySessionCompleteDelegateHandle;
	
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle mStartSessionCompleteDelegateHandle;
};

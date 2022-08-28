// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MultiplayerSessionsSubsystem.generated.h"

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

	//To add to the Online Session Interface delegate list.
	//We'll bind our MultiplayerSessionsSubsystem interal callbacks to these.
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle mCreateSessionCompleteDelegateHandle;
	
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle mFindSessionCompleteDelegateHandle;
	
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle mJoinSessionCompleteDelegateHanlde;
	
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle mDestroySessionCompleteDelegateHandle;
	
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle mStartSessionCompleteDelegateHandle;
};

// Copyright by THQ Nordic

#pragma once
#include "CoreMinimal.h"

#include "GameFramework/GameSession.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerGameSession.generated.h"

class AMultiplayerGameMode;
class SessionsOnlineSubsystem;

UCLASS()
class AMultiplayerGameSession : public AGameSession
{
	GENERATED_BODY()
	
public:
	explicit AMultiplayerGameSession(const FObjectInitializer& objectInitializer);
	
	void BeginPlay() override;
	void BeginDestroy() override;
	
	void CreateSession(bool isLan, uint8 maxNumPlayers, const FString& defaultPlayerName);
	void FindSessions();
	bool JoinSession(const FString& sessionId);
	void DestroySessionAndLeaveGame();
	FNamedOnlineSession* GetCurrentSession() const;
	EOnlineAsyncTaskState::Type GetFindSessionsStatus() const;
	
	void StartGame();
	TSharedPtr<class FOnlineSessionSearch> GetAvailableSessions() const { return m_Sessions; }

	ULocalPlayer* GetLocalPlayer() const;
	
	//Only used for functional test
	void StartSession();
	void EndSession();
	
private:
	void InitializeOnlineSubsystem();
	void UnregisterOnlineSubsystemDelegates() const;
	bool IsLAN() const;
	APlayerController* GetPlayerControllerFromUserId(const FUniqueNetId& userId) const;

	AMultiplayerGameMode* GetGameMode() const;
	FName GetValidMapName(const FName& mapName) const;
	
	void OnCreateSessionComplete(FName sessionName, bool wasSuccessful) const;
	void OnDestroySessionComplete(FName sessionName, bool wasSuccessful) const;
	void OnStartSessionComplete(FName sessionName, bool wasSuccessful) override;
	void OnEndSessionComplete(FName sessionName, bool wasSuccessful) override;
	void OnFindSessionsComplete(TSharedPtr<class FOnlineSessionSearch> sessions, bool wasSuccessful);
	void OnJoinSessionComplete(const FString& travelURL, EOnJoinSessionCompleteResult::Type result);
	
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	FDelegateHandle OnEndSessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	
	TSharedPtr<SessionsOnlineSubsystem> m_OnlineSubsystem;
	TSharedPtr<class FOnlineSessionSearch> m_Sessions;
	bool m_IsLAN;
	FString m_DefaultPlayerName;
};

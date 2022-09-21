// Copyright by THQ Nordic

#pragma once
#include "CoreMinimal.h"
#include "BattleRoyale/core/GameMode/Configuration/MultiplayerConfigurationInfo.h"

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

	void Initialize(const UMultiplayerConfigurationInfo* configuration);
	void CreateSession(bool isLan, uint8 maxNumPlayers, const FString& defaultPlayerName);
	void FindSessions();
	//bool JoinSession(const FString& sessionId);
	void DestroySessionAndLeaveGame();
	FNamedOnlineSession* GetCurrentSession() const;
	EOnlineAsyncTaskState::Type GetFindSessionsStatus() const;
	
	void StartGame();
	//TSharedPtr<class FOnlineSessionSearch> GetAvailableSessions() const { return m_Sessions; }

	ULocalPlayer* GetLocalPlayer() const;
	
	//Only used for functional test
	void StartSession();
	void EndSession();
	
private:
	void InitializeOnlineSubsystem();
	//void UnregisterOnlineSubsystemDelegates() const;
	bool IsLAN() const;
	APlayerController* GetPlayerControllerFromUserId(const FUniqueNetIdRepl& userId) const;

	AMultiplayerGameMode* GetGameMode() const;

	UFUNCTION()
	void OnCreateSessionComplete(bool wasSuccessful);
	UFUNCTION()
	void OnDestroySessionComplete(bool wasSuccessful);
	
	UFUNCTION()
	void OnStartSessionGameComplete(bool wasSuccessful);
	void OnEndSessionGameComplete(bool wasSuccessful);
	void OnFindSessionsComplete(const TArray<FOnlineSessionSearchResult>& sessionsResults, bool wasSuccessful);
	void OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type result);
	
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	FDelegateHandle OnEndSessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	
	//TSharedPtr<SessionsOnlineSubsystem> m_OnlineSubsystem;
	UPROPERTY()
	class UMultiplayerSessionsSubsystem* mMultiplayerSessionsSubsystem;
	bool m_IsLAN;
	FString m_DefaultPlayerName;
	FString mMatchType{"FreeToPlay"};
	
	UPROPERTY()
	const UMultiplayerConfigurationInfo* mConfigurationInfo;
};

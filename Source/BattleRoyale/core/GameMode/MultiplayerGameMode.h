// Copyright by THQ Nordic

#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "SessionsOnlineSubsystem/IOnlineGameSession.h"
#include "GameFramework/GameMode.h"
#include "MultiplayerGameMode.generated.h"

class AMultiplayerGameSession;
class APlayerStart;
class AMSGameSession;

//This class will contain specific rules and stuff related to the multiplayer
//game mode. Things can be shared between different multiplayer games.
UCLASS(minimalapi)
class AMultiplayerGameMode : public AGameMode, public IOnlineGameSession
{
	GENERATED_BODY()

public:
	AMultiplayerGameMode();
	TSubclassOf<AGameSession> GetGameSessionClass() const override;
	
	//TODO this could be part of an interface
	void NotifyPlayerProperlySync(const FString& name);
	virtual void Logout(AController* Exiting) override;
	
	UFUNCTION(BlueprintCallable, Category="GameSession")
    void CreateSession(bool isLAN) override;
	
	void StartSession() override;
	AMultiplayerGameSession* GetGameSession() const;
	
	UFUNCTION(BlueprintCallable, Category="GameSession")
    void EndSession() override;
	UFUNCTION(BlueprintCallable, Category="GameSession")
    void FindSessions() override;
	UFUNCTION(BlueprintCallable, Category="GameSession")
    bool JoinSession(const FString& sessionId) override;
	UFUNCTION(BlueprintCallable, Category="GameSession")
    void DestroySessionAndLeaveGame() override;
	UFUNCTION(BlueprintCallable, Category="GameSession")
    TArray<FOnlineSessionFindResult> GetAvailableSessions() const override;

	UFUNCTION(BlueprintCallable, Category="GameSession")
	bool HasCurrentSession() const override;
	UFUNCTION(BlueprintCallable, Category="GameSession")
	FString GetCurrentSessionIdStr() const override;
	UFUNCTION(BlueprintCallable, Category="GameSession")
	uint8 GetFindSessionsStatus() const override;

	UFUNCTION(BlueprintCallable, Category="Game")
    bool HasLobbyMap() const override;
	UFUNCTION(BlueprintCallable, Category="Game")
    void StartGame() override;

	FString GetGameMapName() const { return GameMapName.ToString(); }
	
	/** Max number of players allowed to play in a multiplayer game together */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Multiplayer")
	uint8 MaxNumPlayers = 4;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Travelling")
	FName LobbyMapName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Travelling")
	FName MainMapName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Travelling")
	FName GameMapName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Travelling")
	FString MapsPath;
private:
	void NotifyAllSyncReady();
	void CheckIfNotifyAllSync(AController* Exiting);
	
	uint8 m_PlayerStartIndex;
	uint8 m_PlayersAlreadySync;
};




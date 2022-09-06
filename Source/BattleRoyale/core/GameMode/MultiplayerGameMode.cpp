// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerGameMode.h"
#include "OnlineSessionSettings.h"
#include "BattleRoyale/core/GameMode/GameSession/MultiplayerGameSession.h"
#include "Configuration/GameModeConfigurationInfo.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

AMultiplayerGameMode::AMultiplayerGameMode() : m_PlayerStartIndex{0}, m_PlayersAlreadySync {0}
{
}

TSubclassOf<AGameSession> AMultiplayerGameMode::GetGameSessionClass() const
{
	return AMultiplayerGameSession::StaticClass();
}

void AMultiplayerGameMode::NotifyPlayerProperlySync(const FString& name)
{
	const auto numPlayers = GetNumPlayers();
	//UE_LOG(LogTemp, Display, TEXT("%s num players %d player %s sync"), __FUNCTION__, numPlayers, *name);
    
	m_PlayersAlreadySync++;
	NotifyAllSyncReady();
}

void AMultiplayerGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if(GameState)
	{
		if(!GEngine)
		{
			return;
		}
		const auto numberOfPlayers = GameState->PlayerArray.Num();
		
		GEngine->AddOnScreenDebugMessage(
			1,
			60.0f,
			FColor::Yellow,
			FString::Printf(TEXT("Players in game %d"), numberOfPlayers));

		const auto playerState = NewPlayer->GetPlayerState<APlayerState>();
		if(!playerState)
		{
			return;
		}
		const auto playerName = playerState->GetPlayerName();
		GEngine->AddOnScreenDebugMessage(
			-1,
			60.0f,
			FColor::Cyan,
			FString::Printf(TEXT("%s has joined the game!"), *playerName));
	}
}

void AMultiplayerGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if(!GEngine)
	{
		return;
	}
	const auto numberOfPlayers = GameState->PlayerArray.Num();
		
	GEngine->AddOnScreenDebugMessage(
		1,
		60.0f,
		FColor::Yellow,
		FString::Printf(TEXT("Players in game %d"), numberOfPlayers - 1));
	
	const auto playerState = Exiting->GetPlayerState<APlayerState>();
	if(!playerState)
	{
		return;
	}
	const auto playerName = playerState->GetPlayerName();
	GEngine->AddOnScreenDebugMessage(
		-1,
		60.0f,
		FColor::Cyan,
		FString::Printf(TEXT("%s has exited the game!"), *playerName));

	CheckIfNotifyAllSync(Exiting);
}

void AMultiplayerGameMode::CreateSession(bool isLAN)
{
	GetGameSession()->CreateSession(
		isLAN,
		mConfigurationInfo->GetMaxNumPlayers(),
		DefaultPlayerName.ToString());
}

void AMultiplayerGameMode::DestroySessionAndLeaveGame()
{
	GetGameSession()->DestroySessionAndLeaveGame();
}

TArray<FOnlineSessionFindResult> AMultiplayerGameMode::GetAvailableSessions() const
{
	TArray<FOnlineSessionFindResult> result;

	auto sessions = GetGameSession()->GetAvailableSessions();
	//UE_LOG(LogTemp, Display, TEXT("%s"), __FUNCTION__);
   		
	if(sessions.IsValid())
	{
		//UE_LOG(LogTemp, Display, TEXT("%s available sessions"), __FUNCTION__);
		
		for(auto&& availableSession : sessions->SearchResults)
		{
			//UE_LOG(LogTemp, Display, TEXT("%s %s"), __FUNCTION__, *availableSession.GetSessionIdStr());
		
			result.Push(FOnlineSessionFindResult(
				availableSession.GetSessionIdStr(),
				availableSession.Session.OwningUserName,
				availableSession.Session.NumOpenPublicConnections,
				availableSession.PingInMs));
		}
	}
	return result;
}

bool AMultiplayerGameMode::HasCurrentSession() const
{
	const auto session = GetGameSession()->GetCurrentSession();
	
	return session != nullptr && session->SessionInfo && session->SessionInfo->IsValid();
}

FString AMultiplayerGameMode::GetCurrentSessionIdStr() const
{
	const auto session = GetGameSession()->GetCurrentSession();

	if(session == nullptr)
	{
		return "";
	}

	return session->GetSessionIdStr();
}

uint8 AMultiplayerGameMode::GetFindSessionsStatus() const
{
	return GetGameSession()->GetFindSessionsStatus();
}

bool AMultiplayerGameMode::HasLobbyMap() const
{
	return !mConfigurationInfo->GetLobbyMapName().IsNone();
}

void AMultiplayerGameMode::StartGame()
{
	GetGameSession()->StartGame();
}

void AMultiplayerGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	if(mConfigurationInfo)
	{
		GetGameSession()->Initialize(mConfigurationInfo);
	}
}

void AMultiplayerGameMode::NotifyAllSyncReady()
{
	const auto numPlayers = GetNumPlayers();
		
	if(numPlayers > 0 && numPlayers == m_PlayersAlreadySync)
	{
		//UE_LOG(LogTemp, Display, TEXT("%s AllPlayersSync!"), __FUNCTION__);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "AllPlayersSync!");
		//auto gameState = GetGameState<AG1RGameState>();
		//gameState->NotifyAllSyncReady_Server();
	}
}

void AMultiplayerGameMode::CheckIfNotifyAllSync(AController* Exiting)
{
	/*const auto character = Cast<IMPSyncCommonCharacter>(Exiting->GetPawn());
	const bool isCharacterProperlySync = character != nullptr && character->IsGameplayReady();
	if(!isCharacterProperlySync)
	{
		NotifyAllSyncReady();
	}*/
}

//This is not used because StartSession and EndSession are
//called automatically by the engine once the map is loaded
//but, let them here because are part of the session
//implementation
void AMultiplayerGameMode::StartSession()
{
	GetGameSession()->StartSession();
}

void AMultiplayerGameMode::EndSession()
{
	GetGameSession()->EndSession();
	
}

void AMultiplayerGameMode::FindSessions()
{
	GetGameSession()->FindSessions();
}

bool AMultiplayerGameMode::JoinSession(const FString& sessionId)
{
	return GetGameSession()->JoinSession(sessionId);
}

AMultiplayerGameSession* AMultiplayerGameMode::GetGameSession() const
{
	return Cast<AMultiplayerGameSession>(GameSession);
}
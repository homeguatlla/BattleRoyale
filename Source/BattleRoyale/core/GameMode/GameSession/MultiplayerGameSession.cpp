// Fill out your copyright notice in the Description page of Project Settings.
#include "MultiplayerGameSession.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "../../../../../Plugins/MultiplayerSessions/Source/MultiplayerSessions/Public/MultiplayerSessionsSubsystem.h"
#include "BattleRoyale/core/GameMode/MultiplayerGameMode.h"
#include "BattleRoyale/core/Utils/UtilsLibrary.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

AMultiplayerGameSession::AMultiplayerGameSession(const FObjectInitializer& objectInitializer)
{	
}

void AMultiplayerGameSession::BeginPlay()
{
	Super::BeginPlay();

	InitializeOnlineSubsystem();
}

void AMultiplayerGameSession::BeginDestroy()
{
	Super::BeginDestroy();
	
	//UnregisterOnlineSubsystemDelegates();
}

void AMultiplayerGameSession::Initialize(const UMultiplayerConfigurationInfo* configuration)
{
	mConfigurationInfo = configuration;
}

void AMultiplayerGameSession::CreateSession(bool isLan, uint8 maxNumPlayers, const FString& defaultPlayerName)
{
	ULocalPlayer* const Player = GetLocalPlayer();
	const bool isPresence = true;//TODO @DG this parameter is now fixed
	m_IsLAN = isLan;
	m_DefaultPlayerName = defaultPlayerName;

	mMultiplayerSessionsSubsystem->CreateSession(maxNumPlayers, mMatchType);
	/*
	m_OnlineSubsystem->CreateSession(
		Player->GetPreferredUniqueNetId().GetUniqueNetId(),
		GameSessionName,
		IsLAN(),
		isPresence,
		maxNumPlayers);*/
}

void AMultiplayerGameSession::DestroySessionAndLeaveGame()
{
	//const FString sessionName(LexToString(GameSessionName));
	//m_OnlineSubsystem->DestroySession(GameSessionName);
	
	mMultiplayerSessionsSubsystem->DestroySession();
}

FNamedOnlineSession* AMultiplayerGameSession::GetCurrentSession() const
{
	const auto onlineSub = IOnlineSubsystem::Get();
	return onlineSub->GetSessionInterface()->GetNamedSession(NAME_GameSession);
}

EOnlineAsyncTaskState::Type AMultiplayerGameSession::GetFindSessionsStatus() const
{
	//return m_OnlineSubsystem->GetFindSessionsStatus();
	return EOnlineAsyncTaskState::Done;
}

void AMultiplayerGameSession::StartGame()
{
	const FString url =
		mConfigurationInfo->GetMapsPath().ToString() +
		utils::UtilsLibrary::GetValidMapName(GetWorld(), mConfigurationInfo->GetGameMapName()).ToString() +
		//"?game=/Game/ThirdPersonCPP/Blueprints/BP_MSGameMode.BP_MSGameMode_C" +
		"?listen";
		
	UE_LOG(LogTemp, Log, TEXT("AMultiplayerGameSession::StartGame URL: %s"), *url);
	GetWorld()->ServerTravel(url, true);
}


void AMultiplayerGameSession::FindSessions()
{
	/*ULocalPlayer* const player = GetLocalPlayer();

	const bool isPresence = true;
	m_OnlineSubsystem->FindSessions(player->GetPreferredUniqueNetId().GetUniqueNetId(), IsLAN(), isPresence);
*/
	mMultiplayerSessionsSubsystem->FindSessions(1000);
}

/*
bool AMultiplayerGameSession::JoinSession(const FString& sessionId)
{
	const auto player = GetLocalPlayer();

	if(!sessionId.IsEmpty())
	{
		const bool result = m_OnlineSubsystem->JoinSession(player->GetPreferredUniqueNetId().GetUniqueNetId(), GameSessionName, sessionId);
		if(!result)
		{
			UE_LOG(LogTemp, Warning, TEXT("AMultiplayerGameSession::JoinSession Error coundn't join to the session"));
		}
		return result;
	}
	
	return false;
}*/

void AMultiplayerGameSession::StartSession()
{
	//m_OnlineSubsystem->StartSession(GameSessionName);
	mMultiplayerSessionsSubsystem->StartSession();
}

void AMultiplayerGameSession::EndSession()
{
	//m_OnlineSubsystem->EndSession(GameSessionName);
}

void AMultiplayerGameSession::OnCreateSessionComplete(bool wasSuccessful)
{		
	if (wasSuccessful)
	{
		const auto map =utils::UtilsLibrary::GetValidMapName(GetWorld(), mConfigurationInfo->GetLobbyMapName());
		UGameplayStatics::OpenLevel(
			GetWorld(),
			map,
			true,
			"listen");
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AMultiplayerGameSession::OnCreateSessionComplete not successful"));
	}
}

void AMultiplayerGameSession::OnDestroySessionComplete(bool wasSuccessful)
{
	if (wasSuccessful)
	{
		UGameplayStatics::OpenLevel(
			GetWorld(),
			utils::UtilsLibrary::GetValidMapName(GetWorld(), mConfigurationInfo->GetMainMapName()),
			true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AMultiplayerGameSession::OnDestroySessionComplete not successful"));
	}
}

void AMultiplayerGameSession::OnFindSessionsComplete(const TArray<FOnlineSessionSearchResult>& sessionsResults, bool wasSuccessful)
{
	if(mMultiplayerSessionsSubsystem == nullptr || !wasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMultiplayerGameSession::OnFindSessionsComplete not successful"));
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
	/*
	if(wasSuccessful)
	{
		FString playerName(m_DefaultPlayerName);
		
		for(auto&& session : sessionsResults)
		{
			const auto player = GetPlayerControllerFromUserId(*session.Session.OwningUserId);
			if(player)
			{
				playerName = player->GetPlayerState<APlayerState>()->GetPlayerName();
			}
		}
		m_Sessions = sessions;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AMultiplayerGameSession::OnFindSessionsComplete not successful"));
	}*/
}

void AMultiplayerGameSession::OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type result)
{	
	if(result == EOnJoinSessionCompleteResult::Type::Success)
	{
		const auto playerController = GetWorld()->GetFirstPlayerController();
		if(!playerController)
		{
			UE_LOG(LogTemp, Warning, TEXT("AMultiplayerGameSession::OnJoinSessionComplete playercontroller null"));
			return;
		}
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
		
		FString travelURL;
		sessionInterface->GetResolvedConnectString(NAME_GameSession, travelURL);
		playerController->ClientTravel(travelURL, ETravelType::TRAVEL_Absolute);		
	}
	else
	{
		const FString resultString = FString(LexToString(result));
		UE_LOG(LogTemp, Warning, TEXT("AMultiplayerGameSession::OnJoinSessionComplete %s"), *resultString);
	}
}

//Only test purposes
//This both methods are overriden and the StartSession is being called
//automatically after game match started, just after ServerTravel, and
//all clients have the map server gone loaded.
void AMultiplayerGameSession::OnStartSessionComplete(bool wasSuccessful)
{
	if (!wasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMultiplayerGameSession::OnDestroySessionComplete not successful"));
	}
}

void AMultiplayerGameSession::OnEndSessionComplete(bool wasSuccessful)
{
	if (wasSuccessful)
	{
		UGameplayStatics::OpenLevel(
			GetWorld(),
			utils::UtilsLibrary::GetValidMapName(GetWorld(), mConfigurationInfo->GetMainMapName()),
			true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AMultiplayerGameSession::OnDestroySessionComplete not successful"));
	}
}
//end only test purposes

AMultiplayerGameMode* AMultiplayerGameSession::GetGameMode() const
{
	const auto gameMode = GetWorld()->GetAuthGameMode();
	return Cast<AMultiplayerGameMode>(gameMode);
}

bool AMultiplayerGameSession::IsLAN() const
{
	return m_IsLAN;// || m_OnlineSubsystem->GetSubsystemName() == "NULL";
}

ULocalPlayer* AMultiplayerGameSession::GetLocalPlayer() const
{
	const auto gameInstance = GetWorld()->GetGameInstance();
	return gameInstance->GetFirstGamePlayer(); //GetWorld()->GetFirstPlayerController()->GetLocalPlayer();
}

APlayerController* AMultiplayerGameSession::GetPlayerControllerFromUserId(const FUniqueNetId& userId) const
{
	auto player = GetPlayerControllerFromNetId(GetWorld(), userId);
	if(!player)
	{
		player = GetWorld()->GetFirstPlayerController();
	}
	
	return player;
}

void AMultiplayerGameSession::InitializeOnlineSubsystem()
{
	const auto gameInstance = GetGameInstance();
	if(!gameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("AMultiplayerGameSession::InitializeOnlineSubsystem MultiplayerSessionsSubsystem not initialized"));
		return;
	}
	mMultiplayerSessionsSubsystem = gameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	
	//m_OnlineSubsystem =  MakeShareable(new SessionsOnlineSubsystem(GetWorld()));
	
	/*OnCreateSessionCompleteDelegateHandle = m_OnlineSubsystem->OnCreateSessionCompleteDelegate().AddUObject(this, &AMultiplayerGameSession::OnCreateSessionComplete);
	OnDestroySessionCompleteDelegateHandle = m_OnlineSubsystem->OnDestroySessionCompleteDelegate().AddUObject(this, &AMultiplayerGameSession::OnDestroySessionComplete);
	OnStartSessionCompleteDelegateHandle = m_OnlineSubsystem->OnStartSessionCompleteDelegate().AddUObject(this, &AMultiplayerGameSession::OnStartSessionComplete);
	OnEndSessionCompleteDelegateHandle = m_OnlineSubsystem->OnEndSessionCompleteDelegate().AddUObject(this, &AMultiplayerGameSession::OnEndSessionComplete);
	OnFindSessionsCompleteDelegateHandle = m_OnlineSubsystem->OnFindSessionsCompleteDelegate().AddUObject(this, &AMultiplayerGameSession::OnFindSessionsComplete);
	OnJoinSessionCompleteDelegateHandle = m_OnlineSubsystem->OnJoinSessionCompleteDelegate().AddUObject(this, &AMultiplayerGameSession::OnJoinSessionComplete);
	*/
	mMultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSessionComplete);
	mMultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessionsComplete);
	mMultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSessionComplete);
	mMultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSessionComplete);
	mMultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySessionComplete);
}
/*
void AMultiplayerGameSession::UnregisterOnlineSubsystemDelegates() const
{
	if(OnCreateSessionCompleteDelegateHandle.IsValid())
	{
		m_OnlineSubsystem->OnCreateSessionCompleteDelegate().Remove(OnCreateSessionCompleteDelegateHandle);
	}
	if(OnDestroySessionCompleteDelegateHandle.IsValid())
	{
		m_OnlineSubsystem->OnDestroySessionCompleteDelegate().Remove(OnDestroySessionCompleteDelegateHandle);
	}

	if(OnStartSessionCompleteDelegateHandle.IsValid())
	{
		m_OnlineSubsystem->OnStartSessionCompleteDelegate().Remove(OnStartSessionCompleteDelegateHandle);
	}

	if(OnEndSessionCompleteDelegateHandle.IsValid())
	{
		m_OnlineSubsystem->OnEndSessionCompleteDelegate().Remove(OnEndSessionCompleteDelegateHandle);
	}
	
	if(OnFindSessionsCompleteDelegateHandle.IsValid())
	{
		m_OnlineSubsystem->OnFindSessionsCompleteDelegate().Remove(OnFindSessionsCompleteDelegateHandle);
	}
	if(OnJoinSessionCompleteDelegateHandle.IsValid())
	{
		m_OnlineSubsystem->OnJoinSessionCompleteDelegate().Remove(OnJoinSessionCompleteDelegateHandle);
	}
}*/
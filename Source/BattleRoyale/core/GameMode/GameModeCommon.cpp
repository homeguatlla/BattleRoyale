// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeCommon.h"

#include "AbilitySystemGlobals.h"
#include "IGameState.h"
#include "OnlineSubsystem.h"
#include "BattleRoyale/core/PlayerController/IPlayerController.h"
#include "GameFramework/GameStateBase.h"
#include "GameSession/MultiplayerGameSession.h"
#include "PlayerState/PlayerStateBase.h"


//Cuando se ejecuta el BeginPlay, ya se ha ejecutado el GenericPlayerInitialization para el server
//así que aquí no se puede inicializar nada que pueda necesitar el propio server en ese método.
//Las cosas más genéricas inicializarlas en InitGame.
//En este punto tenemos ya el GameState creado no antes.
void AGameModeCommon::BeginPlay()
{
	Super::BeginPlay();
}

void AGameModeCommon::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	
	DoInitGame();
	//This line is important to make ability system target data works.
	UAbilitySystemGlobals::Get().InitGlobalData();
}

//Este método solo se ejecuta cuando un jugador se incorpora a una partida ya empezada
//el GenericPlayerInitialization si se ejecuta en ambos casos y antes que el PostLogin
void AGameModeCommon::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	UE_LOG(LogGameMode, Log, TEXT("AGameModeCommon::PostLogin new player"));
}

void AGameModeCommon::GenericPlayerInitialization(AController* controller)
{
	Super::GenericPlayerInitialization(controller);

	const auto gameState = GetGameState();
	if(gameState != nullptr &&  gameState->IsGameReadyToStart())
	{
		//in case a player joins to the game when is ready to start (because for example the countdown finished)
		return;
	}
	
	//mPlayerControllers.Add(controller);
	
	DisableControllerInput(controller);

	DoGenereicPlayerInitialization(controller);
}

IIGameState* AGameModeCommon::GetGameState() const
{
	if(GameState != nullptr && GameState->Implements<UIGameState>())
	{
		return Cast<IIGameState>(GameState);
	}
	return nullptr;
}

void AGameModeCommon::OnNewKill(IIPlayerState* killerPlayerState, IIPlayerState* victimPlayerState)
{
	if(killerPlayerState)
	{
		killerPlayerState->AddKill();
		killerPlayerState->NotifyNumKillsToSelf();
		NotifyNewKillToAll(victimPlayerState, killerPlayerState);
	}

	if(victimPlayerState)
	{
		victimPlayerState->NotifyGameOverServer(false, false);
	}

	GetGameState()->NotifyNumTeamsAndPlayersAlive();
}

bool AGameModeCommon::CanPlayerCauseDamageTo(const IIPlayerState* causerPlayerState,
                                             const IIPlayerState* victimPlayerState)
{
	return causerPlayerState->GetTeamId() != victimPlayerState->GetTeamId();
}

bool AGameModeCommon::HasCurrentGameSession() const
{
	return Cast<AMultiplayerGameMode>(this)->HasCurrentSession();
}

void AGameModeCommon::DestroyGameSession()
{
	const auto onlineSub = IOnlineSubsystem::Get();
	onlineSub->GetSessionInterface()->DestroySession(NAME_GameSession);
}

void AGameModeCommon::NotifyNewKillToAll(const IIPlayerState* victimPlayerState, const IIPlayerState* playerStateKiller) const
{
	if(victimPlayerState == nullptr)
	{
		return;
	}
	
	playerStateKiller->NotifyAnnouncementOfNewDeathToAll(
		playerStateKiller->GetPlayerNickName(),
		victimPlayerState->GetPlayerNickName());
}

bool AGameModeCommon::ReadyToStartMatch_Implementation()
{
	bool isReadyToStartMatch = Super::ReadyToStartMatch_Implementation();
	
	const auto gameState = GetGameState();
	if(gameState != nullptr)
	{
		isReadyToStartMatch = isReadyToStartMatch && gameState->IsGameReadyToStart();
	}
	
	return isReadyToStartMatch;
}

void AGameModeCommon::DisableControllerInput(AController* controller) const
{
	const auto playerController = GetPlayerController(controller);
	if(playerController)
	{
		playerController->EnableInput(false);
	}
}

IIPlayerController* AGameModeCommon::GetPlayerController(AController* controller) const
{
	if(controller != nullptr && controller->Implements<UIPlayerController>())
	{
		return Cast<IIPlayerController>(controller);
	}

	return nullptr;
}
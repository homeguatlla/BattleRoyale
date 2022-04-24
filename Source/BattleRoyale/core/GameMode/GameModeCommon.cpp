// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeCommon.h"

#include "IGameState.h"
#include "BattleRoyale/core/PlayerController/IPlayerController.h"
#include "GameFramework/GameStateBase.h"
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

void AGameModeCommon::OnNewKill(const APlayerController* killerController, const APlayerController* victimController)
{
	//TODO refactorizar y poner dos métodos.
	
	const auto playerStateKiller = killerController->GetPlayerState<APlayerStateBase>();
	if(playerStateKiller && playerStateKiller->Implements<UIPlayerState>())
	{
		playerStateKiller->AddKill();
		playerStateKiller->NotifyNumKillsToSelf();
		NotifyNewKillToAll(victimController, playerStateKiller);
	}

	const auto playerStateVictim = victimController->GetPlayerState<APlayerStateBase>();
	if(playerStateVictim && playerStateVictim->Implements<UIPlayerState>())
	{
		playerStateVictim->NotifyGameOver();
	}
}

bool AGameModeCommon::CanPlayerCauseDamageTo(const APlayerController* killerController,
	const APlayerController* victimController)
{
	const auto killerPlayerState = killerController->GetPlayerState<IIPlayerState>();
	const auto victimPlayerState = victimController->GetPlayerState<IIPlayerState>();
	
	return killerPlayerState->GetTeamId() != victimPlayerState->GetTeamId();
}

void AGameModeCommon::NotifyNewKillToAll(const APlayerController* victimController, APlayerStateBase* const playerStateKiller) const
{
	const auto playerStateVictim = victimController->GetPlayerState<APlayerStateBase>();
	if(playerStateVictim == nullptr || !playerStateVictim->Implements<UIPlayerState>())
	{
		return;
	}
	
	const auto playerStateVictimInterface = Cast<IIPlayerState>(playerStateVictim);		
	const auto playerStateKillerInterface = Cast<IIPlayerState>(playerStateKiller);
		
	playerStateKillerInterface->NotifyAnnouncementOfNewDeathToAll(
		playerStateKillerInterface->GetPlayerNickName(),
		playerStateVictimInterface->GetPlayerNickName());
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
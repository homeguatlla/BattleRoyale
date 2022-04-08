// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleRoyaleGameMode.h"

#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/PlayerState/PlayerStateBase.h"
#include "BattleRoyale/core/HUD/BattleRoyaleHUD.h"
#include "BattleRoyale/core/PlayerController/PlayerControllerBase.h"
#include "BattleRoyale/core/Utils/TeamSelectionStrategies/SimpleTeamSelectionStrategy.h"
#include "GameFramework/GameState.h"
#include "GameRules/StartCountDownRule.h"


ABattleRoyaleGameMode::ABattleRoyaleGameMode()
	: Super()
{
	DefaultPawnClass = mCharacter;

	// use our custom HUD class
	HUDClass = ABattleRoyaleHUD::StaticClass();

	//FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FGameModeEvents::OnGameModeMatchStateSetEvent().AddUObject(this, &ABattleRoyaleGameMode::OnMatchStateChanged);
}

//Cuando se ejecuta el BeginPlay, ya se ha ejecutado el GenericPlayerInitialization para el server
//así que aquí no se puede inicializar nada que pueda necesitar el propio server en ese método.
//Las cosas más genéricas inicializarlas en InitGame.
//En este punto tenemos ya el GameState creado no antes.
void ABattleRoyaleGameMode::BeginPlay()
{
	Super::BeginPlay();

	InitializeGameRules();
}

bool ABattleRoyaleGameMode::ReadyToStartMatch_Implementation()
{
	bool isReadyToStartMatch = Super::ReadyToStartMatch_Implementation();

	if(mGameRules)
	{
		mGameRules->Execute();
	}
	
	const auto gameState = GetGameState();
	if(gameState != nullptr)
	{
		isReadyToStartMatch = isReadyToStartMatch && gameState->HasGameStarted();
	}
	
	return isReadyToStartMatch;
}

//Este método solo se ejecuta cuando un jugador se incorpora a una partida ya empezada
//el GenericPlayerInitialization si se ejecuta en ambos casos y antes que el PostLogin
void ABattleRoyaleGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	UE_LOG(LogGameMode, Log, TEXT("ABattleRoyaleGameMode::PostLogin new player"));
}

void ABattleRoyaleGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	
	InitializeTeamSelectionStrategy();
}

void ABattleRoyaleGameMode::GenericPlayerInitialization(AController* controller)
{
	Super::GenericPlayerInitialization(controller);

	const auto gameState = GetGameState();
	if(gameState != nullptr &&  gameState->HasGameStarted())
	{
		//in case a player joins to the game and countdown finished
		return;
	}
	
	ApplyTeamSelectionStrategy(controller);
	
	mPlayerControllers.Add(controller);
	
	DisableControllerInput(controller);
	
	UE_LOG(LogGameMode, Log, TEXT("ABattleRoyaleGameMode::GenericPlayerInitialization num players = %d"), mPlayerControllers.Num());
}

void ABattleRoyaleGameMode::OnMatchStateChanged(FName matchState)
{
	UE_LOG(LogGameMode, Log, TEXT("ABattleRoyaleGameMode::OnMatchStateSet %s"), *matchState.ToString());
}

void ABattleRoyaleGameMode::OnNewKill(const APlayerController* killerController, const APlayerController* victimController)
{
	const auto playerStateKiller = killerController->GetPlayerState<APlayerStateBase>();
	if(playerStateKiller && playerStateKiller->Implements<UIPlayerState>())
	{
		playerStateKiller->AddKill();
		playerStateKiller->NotifyNumKillsToSelf();
		NotifyNewKillToAll(victimController, playerStateKiller);		
	}

	mGameRules->Execute();
}

bool ABattleRoyaleGameMode::CanPlayerCauseDamageTo(const APlayerController* killerController,
	const APlayerController* victimController)
{
	const auto killerPlayerState = killerController->GetPlayerState<IIPlayerState>();
	const auto victimPlayerState = victimController->GetPlayerState<IIPlayerState>();
	
	return killerPlayerState->GetTeamId() != victimPlayerState->GetTeamId();
}

void ABattleRoyaleGameMode::ApplyTeamSelectionStrategy(const AController* controller) const
{
	if(mTeamSelectionStrategy != nullptr)
	{
		const auto playerState = controller->GetPlayerState<APlayerStateBase>();
		if(playerState && playerState->Implements<UIPlayerState>())
		{
			const auto playerStateInterface = Cast<IIPlayerState>(playerState);
			const auto teamId = mTeamSelectionStrategy->GetNextTeamId();
			UE_LOG(LogGameMode, Display, TEXT("ABattleRoyaleGameMode::ApplyTeamSelectionStrategy Team id: %d"), teamId);
			playerStateInterface->SetTeamId(teamId);
		}
	}
}

void ABattleRoyaleGameMode::DisableControllerInput(AController* controller) const
{
	const auto playerController = GetPlayerController(controller);
	if(playerController)
	{
		playerController->EnableInput(false);
	}
}

IIPlayerController* ABattleRoyaleGameMode::GetPlayerController(AController* controller) const
{
	if(controller != nullptr && controller->Implements<UIPlayerController>())
	{
		return Cast<IIPlayerController>(controller);
	}

	return nullptr;
}

IIGameState* ABattleRoyaleGameMode::GetGameState() const
{
	if(GameState != nullptr && GameState->Implements<UIGameState>())
	{
		return Cast<IIGameState>(GameState);
	}
	return nullptr;
}

void ABattleRoyaleGameMode::InitializeGameRules()
{
	const auto startCountdownRule = NewObject<UStartCountDownRule>();
	
	TScriptInterface<IIGameState> gameStateInterface;
	gameStateInterface.SetObject(GameState);
	gameStateInterface.SetInterface(GetGameState());

	startCountdownRule->Initialize(gameStateInterface);
	startCountdownRule->SetCountdownTimeToStartGame(mCountdownTimeToStartGame);
	
	mGameRules = NewObject<UGameRules>();
	mGameRules->AddRule(startCountdownRule);
}

void ABattleRoyaleGameMode::InitializeTeamSelectionStrategy()
{
	mTeamSelectionStrategy = NewObject<USimpleTeamSelectionStrategy>();
	mTeamSelectionStrategy->Initialize(mNumPlayersPerTeam);
}

void ABattleRoyaleGameMode::NotifyNewKillToAll(const APlayerController* victimController, APlayerStateBase* const playerStateKiller) const
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

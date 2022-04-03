// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleRoyaleGameMode.h"

#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/PlayerState/PlayerStateBase.h"
#include "BattleRoyale/core/HUD/BattleRoyaleHUD.h"
#include "BattleRoyale/core/PlayerController/PlayerControllerBase.h"
#include "GameFramework/GameState.h"
#include "GameRules/StartGameRule.h"

ABattleRoyaleGameMode::ABattleRoyaleGameMode()
	: Super()
{
	DefaultPawnClass = mCharacter;

	// use our custom HUD class
	HUDClass = ABattleRoyaleHUD::StaticClass();

	//FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FGameModeEvents::OnGameModeMatchStateSetEvent().AddUObject(this, &ABattleRoyaleGameMode::OnMatchStateChanged);
}

void ABattleRoyaleGameMode::BeginPlay()
{
	Super::BeginPlay();
	InitializeGameRules();
}

bool ABattleRoyaleGameMode::ReadyToStartMatch_Implementation()
{
	bool isReadyToStartMatch = Super::ReadyToStartMatch_Implementation();
	
	//If timer is 0 or teams are created then ready to start match.
	const auto gameState = GetGameState();
	if(gameState != nullptr)
	{
		isReadyToStartMatch = isReadyToStartMatch && gameState->DidCountdownFinish();
	}

	if(mGameRules)
	{
		mGameRules->Execute();
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

void ABattleRoyaleGameMode::GenericPlayerInitialization(AController* controller)
{
	Super::GenericPlayerInitialization(controller);

	const auto gameState = GetGameState();
	if(gameState != nullptr &&  gameState->DidCountdownStart() && gameState->DidCountdownFinish())
	{
		//in case a player joins to the game and countdown finished
		return;
	}

	TryToStartCountdown();

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
	//TODO validar que no sean del mismo equipo por ejemplo
	return true;
}

void ABattleRoyaleGameMode::TryToStartCountdown() const
{
	const auto gameState = GetGameState();
	
	if(gameState != nullptr)
	{
		if(!gameState->DidCountdownStart())
		{
			gameState->StartCountdownServer(mCountdownTimeToStartGame);
		}
	}
	else
	{
		UE_LOG(LogGameMode, Error, TEXT("ABattleRoyaleGameMode::TryToStartCountdown Is GameState implementing IIGameState?"));
	}
}

void ABattleRoyaleGameMode::ApplyTeamSelectionStrategy(const AController* controller)
{
	const auto playerState = controller->GetPlayerState<APlayerStateBase>();
	if(playerState && playerState->Implements<UIPlayerState>())
	{
		const auto playerStateInterface = Cast<IIPlayerState>(playerState);
		playerStateInterface->SetTeamId(mLastTeamId);
		mLastTeamId++;
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
	const auto startGameRule = NewObject<UStartGameRule>();
	
	TScriptInterface<IIGameState> gameStateInterface;
	gameStateInterface.SetObject(GameState);
	gameStateInterface.SetInterface(GetGameState());

	startGameRule->Initialize(gameStateInterface);
	mGameRules = NewObject<UGameRules>();
	mGameRules->AddRule(startGameRule);
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

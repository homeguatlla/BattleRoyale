// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleRoyaleGameMode.h"

#include "BattleRoyaleGameState.h"
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/HUD/BattleRoyaleHUD.h"
#include "BattleRoyale/core/PlayerController/BattleRoyalePlayerController.h"
#include "GameFramework/GameState.h"

ABattleRoyaleGameMode::ABattleRoyaleGameMode()
	: Super()
{
	DefaultPawnClass = mCharacter;

	// use our custom HUD class
	HUDClass = ABattleRoyaleHUD::StaticClass();

	//FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FGameModeEvents::OnGameModeMatchStateSetEvent().AddUObject(this, &ABattleRoyaleGameMode::OnMatchStateChanged);
	
}
void ABattleRoyaleGameMode::OnMatchStateChanged(FName matchState)
{
	UE_LOG(LogTemp, Warning, TEXT("ABattleRoyaleGameMode::OnMatchStateSet %s"), *matchState.ToString());
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
	return isReadyToStartMatch;
}

//Este método solo se ejecuta cuando un jugador se incorpora a una partida ya empezada
//el GenericPlayerInitialization si se ejecuta en ambos casos y antes que el PostLogin
void ABattleRoyaleGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	UE_LOG(LogTemp, Warning, TEXT("ABattleRoyaleGameMode::PostLogin new player"));
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
		UE_LOG(LogTemp, Error, TEXT("ABattleRoyaleGameMode::TryToStartCountdown Is GameState implementing IIGameState?"));
	}
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
	
	mPlayerControllers.Add(controller);
	
	DisableControllerInput(controller);
	
	UE_LOG(LogTemp, Warning, TEXT("ABattleRoyaleGameMode::GenericPlayerInitialization num players = %d"), mPlayerControllers.Num());
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
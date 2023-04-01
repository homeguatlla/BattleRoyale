// Fill out your copyright notice in the Description page of Project Settings.


#include "SinglePlayerGameState.h"
#include "SinglePlayerGameMode.h"
#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "BattleRoyale/core/Utils/FSM/StatesMachineFactory.h"
#include "Net/UnrealNetwork.h"


ASinglePlayerGameState::ASinglePlayerGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
}

void ASinglePlayerGameState::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority())
	{
		AddStatesMachineServer(mStatesMachineController, mGameStateFSMContext);
		SetActorTickEnabled(true);
	}
}

void ASinglePlayerGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
}

void ASinglePlayerGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(HasAuthority())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "ABattleRoyaleGameState::Tick!");
		mStatesMachineController.Update(DeltaSeconds);
	}
}

void ASinglePlayerGameState::StartGameServer()
{
	MulticastGameStarted();
	
	//For all players execute the gameStarted
	PerformActionForEachPlayerState(
		[](IIPlayerState* playerState) -> bool
		{
			playerState->OnGameStartedServer();
			return false;
		});
}

bool ASinglePlayerGameState::HasGameStarted() const
{
	return mStatesMachineController.GetCurrentStateID(static_cast<int>(FSMType::SINGLEPLAYER_GAMEMODE)) == BRModeFSM::SinglePlayerState::STATE_GAMELOOP;
}

bool ASinglePlayerGameState::HasMatchEnded() const
{
	return Super::HasMatchEnded();
}

void ASinglePlayerGameState::MatchEndServer()
{
}

void ASinglePlayerGameState::PlayerInteraction(const APlayerController* playerController, const FString& action)
{
}

void ASinglePlayerGameState::NotifyGameModeConfigurationInfo(const UGameModeConfigurationInfo* configurationInfo)
{
}

void ASinglePlayerGameState::NotifyNumTeamsAndPlayersAlive()
{
}

bool ASinglePlayerGameState::IsGameReadyToStart() const
{
	return true;
}

void ASinglePlayerGameState::PerformActionForEachPlayerState(
	std::function<bool(IIPlayerState* playerState)> action) const
{
	for(const auto playerState : PlayerArray)
	{
		if(playerState->Implements<UIPlayerState>())
		{
			const auto specificPlayerState = Cast<IIPlayerState>(playerState);
			if(action(specificPlayerState))
				break;
		}
	}
}

void ASinglePlayerGameState::MulticastGameStarted_Implementation()
{
	//To notify HUD
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
	gameInstance->GetEventDispatcher()->OnGameStarted.Broadcast();
}

void ASinglePlayerGameState::AddStatesMachineServer(
	StatesMachineController<BRModeFSM::SinglePlayerState, BRModeFSM::SinglePlayerContext>& fsmController,
	std::shared_ptr<BRModeFSM::SinglePlayerContext>& fsmContext)
{
	const auto gameMode = Cast<ASinglePlayerGameMode>(GetWorld()->GetAuthGameMode());
	fsmContext = std::make_shared<BRModeFSM::SinglePlayerContext>(
		GetWorld(),
		gameMode,
		this);
	
	BattleRoyale::StatesMachineFactory factory;

	fsmController.AddMachine(
	std::move(factory.CreateModeFSMSinglePlayer(
		FSMType::SINGLEPLAYER_GAMEMODE,
		fsmContext)));
			
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/GameMode/MultiplayerGameState.h"
#include "BattleRoyale/core/Utils/FSM/StatesMachineController.h"
#include "FSM/SinglePlayerContext.h"
#include "FSM/States/SinglePlayerStates.h"
#include "SinglePlayerGameState.generated.h"


UCLASS()
class BATTLEROYALE_API ASinglePlayerGameState : public AGameState, public IIGameState
{
	GENERATED_BODY()

public:
	ASinglePlayerGameState();
	
	virtual void StartGameServer() override;
	virtual bool HasGameStarted() const override;
	virtual bool HasMatchEnded() const override;
	virtual void MatchEndServer() override;
	virtual void PlayerInteraction(const APlayerController* playerController, const FString& action) override;
	virtual void NotifyGameModeConfigurationInfo(const UGameModeConfigurationInfo* configurationInfo) override;
	virtual void NotifyNumTeamsAndPlayersAlive() override;
	virtual bool IsGameReadyToStart() const override;

protected:
	virtual void BeginPlay() override;
	
private:
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void Tick(float DeltaSeconds);
	void PerformActionForEachPlayerState(std::function<bool(IIPlayerState* playerState)> action) const;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastGameStarted();
	
	//TODO no queremos heredar de multiplayer. Igual el single player debería ser el cooperativo!!! ojo
	//porque el cooperativo es un single player que adicionalmente se puede agregar más players.
	virtual void AddStatesMachineServer(
		StatesMachineController<BRModeFSM::SinglePlayerState, BRModeFSM::SinglePlayerContext>& fsmController,
		std::shared_ptr<BRModeFSM::SinglePlayerContext>& fsmContext);

	//States machine to control the game state
	StatesMachineController<BRModeFSM::SinglePlayerState, BRModeFSM::SinglePlayerContext> mStatesMachineController;
	std::shared_ptr<BRModeFSM::SinglePlayerContext> mGameStateFSMContext;
};

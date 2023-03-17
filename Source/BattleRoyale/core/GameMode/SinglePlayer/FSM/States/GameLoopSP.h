#pragma once
#include "SinglePlayerStates.h"
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/GameRules/GameRules.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseState.h"


namespace BRModeFSM
{
	class SinglePlayerContext;

	class GameLoopSP : public core::utils::FSM::BaseState<SinglePlayerState, SinglePlayerContext>
	{
	public:
		
		GameLoopSP();
		virtual ~GameLoopSP() override = default;

		virtual SinglePlayerState GetID() const override { return SinglePlayerState::STATE_GAMELOOP; }
		
		virtual void OnInit() override;
		virtual void OnEnter(float deltaTime) override;
		virtual void OnUpdate(float deltaTime) override;

	private:
		void InitializeGameRules();
		
		GameRules mGameRules;
		IIGameState* mGameState = nullptr;
	};
};

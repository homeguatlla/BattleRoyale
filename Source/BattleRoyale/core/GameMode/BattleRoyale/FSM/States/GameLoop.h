#pragma once
#include "BattleRoyaleStates.h"
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/GameRules/GameRules.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseState.h"


namespace BRModeFSM
{
	class BattleRoyaleContext;

	class GameLoop : public core::utils::FSM::BaseState<BattleRoyaleState, BattleRoyaleContext>
	{
	public:
		
		GameLoop();
		virtual ~GameLoop() = default;

		BattleRoyaleState GetID() const override { return BattleRoyaleState::STATE_GAMELOOP; }
		
		virtual void OnInit() override;
		virtual void OnEnter(float deltaTime) override;
		virtual void OnUpdate(float deltaTime) override;

	private:
		GameRules mGameRules;
		IIGameState* mGameState;
	};
};

#pragma once
#include "BattleRoyaleStates.h"
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
		
		void OnInit() override;
	};
};

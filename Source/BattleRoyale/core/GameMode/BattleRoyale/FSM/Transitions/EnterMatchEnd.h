#pragma once
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/BattleRoyaleStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseTransition.h"

namespace BRModeFSM
{
	class BattleRoyaleContext;

	class EnterMatchEnd : public core::utils::FSM::BaseTransition<BattleRoyaleState, BattleRoyaleContext>
	{
	public:
		EnterMatchEnd(StatePtr origin, StatePtr destination);
		virtual ~EnterMatchEnd() = default;

		void OnInit() override;
		bool CanPerformTransition() const override;
		
	private:
		IIGameState* mGameState;
	};
};

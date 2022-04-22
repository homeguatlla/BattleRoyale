#pragma once
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/BattleRoyaleStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseTransition.h"

namespace BRModeFSM
{
	class BattleRoyaleContext;

	class EnterSynchronize : public core::utils::FSM::BaseTransition<BattleRoyaleState, BattleRoyaleContext>
	{
	public:
		EnterSynchronize(StatePtr origin, StatePtr destination);
		virtual ~EnterSynchronize() = default;

		void OnInit() override;
		bool CanPerformTransition() const override;

	private:
		IIGameState* mGameState;
	};
};

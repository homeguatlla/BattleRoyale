#pragma once
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/BattleRoyaleStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseTransition.h"

class ABattleRoyaleGameState;

namespace BRModeFSM
{
	class BattleRoyaleContext;

	class EnterCountdown : public core::utils::FSM::BaseTransition<BattleRoyaleState, BattleRoyaleContext>
	{
	public:
		EnterCountdown(StatePtr origin, StatePtr destination);
		virtual ~EnterCountdown() = default;

		void OnInit() override;
		bool CanPerformTransition() const override;

	private:
		const ABattleRoyaleGameState* mGameState;
	};
};

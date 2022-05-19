#pragma once
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/BattleRoyaleStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseTransition.h"

namespace BRModeFSM
{
	class BattleRoyaleContext;

	class EnterReset : public core::utils::FSM::BaseTransition<BattleRoyaleState, BattleRoyaleContext>
	{
	public:
		EnterReset(StatePtr origin, StatePtr destination);
		virtual ~EnterReset() = default;

		void OnInit() override;
		bool CanPerformTransition() const override;
	};
};

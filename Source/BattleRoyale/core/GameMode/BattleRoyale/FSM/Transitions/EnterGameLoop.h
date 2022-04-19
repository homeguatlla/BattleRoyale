#pragma once
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/BattleRoyaleStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseTransition.h"

namespace BRModeFSM
{
	class BattleRoyaleContext;

	class EnterGameLoop : public core::utils::FSM::BaseTransition<BattleRoyaleState, BattleRoyaleContext>
	{
	public:
		EnterGameLoop(StatePtr origin, StatePtr destination);
		virtual ~EnterGameLoop() = default;

		void OnInit() override;
		bool CanPerformTransition() const override;
	};
};

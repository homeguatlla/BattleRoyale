#pragma once
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/BattleRoyaleStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseTransition.h"

namespace BRModeFSM
{
	class BattleRoyaleContext;
	class CharacterContext;

	class EnterIdle : public core::utils::FSM::BaseTransition<BattleRoyaleState, BattleRoyaleContext>
	{
	public:
		EnterIdle(StatePtr origin, StatePtr destination);
		virtual ~EnterIdle() = default;

		void OnInit() override;
		bool CanPerformTransition() const override;
	};
};

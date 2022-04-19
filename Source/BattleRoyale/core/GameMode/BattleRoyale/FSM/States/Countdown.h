#pragma once
#include "BattleRoyaleStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseState.h"


namespace BRModeFSM
{
	class BattleRoyaleContext;

	class Countdown : public core::utils::FSM::BaseState<BattleRoyaleState, BattleRoyaleContext>
	{
	public:
		Countdown();
		virtual ~Countdown() = default;

		BattleRoyaleState GetID() const override { return BattleRoyaleState::STATE_COUNTDOWN; }
		
		void OnInit() override;
	};
};

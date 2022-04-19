#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/Transitions/EnterCountdown.h"

namespace BRModeFSM
{
	EnterCountdown::EnterCountdown(StatePtr origin, StatePtr destination) :
		core::utils::FSM::BaseTransition<BattleRoyaleState, BattleRoyaleContext>(origin, destination)
	{
	}

	void EnterCountdown::OnInit()
	{
		
	}

	bool EnterCountdown::CanPerformTransition() const
	{
		return true;
	}
};
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/Transitions/EnterIdle.h"

namespace BRModeFSM
{
	EnterIdle::EnterIdle(StatePtr origin, StatePtr destination) :
		core::utils::FSM::BaseTransition<BattleRoyaleState, BattleRoyaleContext>(origin, destination)
	{
	}

	void EnterIdle::OnInit()
	{
		
	}

	bool EnterIdle::CanPerformTransition() const
	{
		return true;
	}
};
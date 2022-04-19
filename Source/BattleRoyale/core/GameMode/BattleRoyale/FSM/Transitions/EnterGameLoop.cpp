#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/Transitions/EnterGameLoop.h"

namespace BRModeFSM
{
	EnterGameLoop::EnterGameLoop(StatePtr origin, StatePtr destination) :
		core::utils::FSM::BaseTransition<BattleRoyaleState, BattleRoyaleContext>(origin, destination)
	{
	}

	void EnterGameLoop::OnInit()
	{
		
	}

	bool EnterGameLoop::CanPerformTransition() const
	{
		return true;
	}
};
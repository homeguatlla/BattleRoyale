
#include "EnterGameLoop.h"

namespace BRPlayerStateFSM
{
	EnterGameLoop::EnterGameLoop(StatePtr origin, StatePtr destination) :
		core::utils::FSM::BaseTransition<PlayerStateState, PlayerStateContext>(origin, destination)
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
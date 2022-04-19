#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/Transitions/EnterSynchronize.h"

namespace BRModeFSM
{
	EnterSynchronize::EnterSynchronize(StatePtr origin, StatePtr destination) :
		core::utils::FSM::BaseTransition<BattleRoyaleState, BattleRoyaleContext>(origin, destination)
	{
	}

	void EnterSynchronize::OnInit()
	{
		
	}

	bool EnterSynchronize::CanPerformTransition() const
	{
		return true;
	}
};

#include "EnterStats.h"

#include "BattleRoyale/core/GameMode/PlayerState/FSM/PlayerStateContext.h"

namespace BRPlayerStateFSM
{
	EnterStats::EnterStats(StatePtr origin, StatePtr destination) :
		core::utils::FSM::BaseTransition<PlayerStateState, PlayerStateContext>(origin, destination)
	{
	}

	void EnterStats::OnInit()
	{
		
	}

	bool EnterStats::CanPerformTransition() const
	{
		return GetContext()->IsTimeOver();
	}	
};
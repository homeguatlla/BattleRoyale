
#include "ServerEnterStats.h"

#include "BattleRoyale/core/GameMode/PlayerState/FSM/PlayerStateContext.h"

namespace BRPlayerStateFSM
{
	ServerEnterStats::ServerEnterStats(StatePtr origin, StatePtr destination) :
		core::utils::FSM::BaseTransition<PlayerStateState, PlayerStateContext>(origin, destination)
	{
	}

	void ServerEnterStats::OnInit()
	{
		
	}

	bool ServerEnterStats::CanPerformTransition() const
	{
		return GetContext()->IsTimeOver();
	}	
};

#include "ServerEnterGameOver.h"

#include "BattleRoyale/core/GameMode/PlayerState/FSM/PlayerStateContext.h"

namespace BRPlayerStateFSM
{
	ServerEnterGameOver::ServerEnterGameOver(StatePtr origin, StatePtr destination) :
		core::utils::FSM::BaseTransition<PlayerStateState, PlayerStateContext>(origin, destination)
	{
	}

	void ServerEnterGameOver::OnInit()
	{
		
	}

	bool ServerEnterGameOver::CanPerformTransition() const
	{
		return GetContext()->IsTimeOver();
	}	
};
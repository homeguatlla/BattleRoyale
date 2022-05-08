
#include "ServerEnterInit.h"

#include "BattleRoyale/core/GameMode/PlayerState/FSM/PlayerStateContext.h"

namespace BRPlayerStateFSM
{
	ServerEnterInit::ServerEnterInit(StatePtr origin, StatePtr destination) :
		core::utils::FSM::BaseTransition<PlayerStateState, PlayerStateContext>(origin, destination)
	{
	}

	void ServerEnterInit::OnInit()
	{
		mCharacter = GetContext()->GetCharacter();
	}

	bool ServerEnterInit::CanPerformTransition() const
	{
		return false;
	}	
};
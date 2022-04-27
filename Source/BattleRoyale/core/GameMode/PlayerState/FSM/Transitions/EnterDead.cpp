
#include "EnterDead.h"

#include "BattleRoyale/core/GameMode/PlayerState/FSM/PlayerStateContext.h"

namespace BRPlayerStateFSM
{
	EnterDead::EnterDead(StatePtr origin, StatePtr destination) :
		core::utils::FSM::BaseTransition<PlayerStateState, PlayerStateContext>(origin, destination)
	{
	}

	void EnterDead::OnInit()
	{
		mPlayerState = GetContext()->GetPlayerState();
	}

	bool EnterDead::CanPerformTransition() const
	{
		return !mPlayerState->IsAlive();
	}	
};
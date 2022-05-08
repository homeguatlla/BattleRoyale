
#include "EnterVictory.h"

#include "BattleRoyale/core/GameMode/PlayerState/FSM/PlayerStateContext.h"

namespace BRPlayerStateFSM
{
	EnterVictory::EnterVictory(StatePtr origin, StatePtr destination) :
		core::utils::FSM::BaseTransition<PlayerStateState, PlayerStateContext>(origin, destination)
	{
	}

	void EnterVictory::OnInit()
	{
		mPlayerState = GetContext()->GetPlayerState();
	}

	bool EnterVictory::CanPerformTransition() const
	{
		return mPlayerState->DidPlayerWin();
	}	
};
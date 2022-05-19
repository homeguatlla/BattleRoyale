
#include "EnterRestart.h"

#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/PlayerStateContext.h"

namespace BRPlayerStateFSM
{
	EnterRestart::EnterRestart(StatePtr origin, StatePtr destination) :
		core::utils::FSM::BaseTransition<PlayerStateState, PlayerStateContext>(origin, destination)
	{
	}

	void EnterRestart::OnInit()
	{
	}

	bool EnterRestart::CanPerformTransition() const
	{
		return GetContext()->GetPlayerState()->GetPlayerInteraction().Len() > 0;
	}	
};

#include "ClientEnterInit.h"

#include "BattleRoyale/core/GameMode/PlayerState/FSM/PlayerStateContext.h"

namespace BRPlayerStateFSM
{
	ClientEnterInit::ClientEnterInit(StatePtr origin, StatePtr destination) :
		core::utils::FSM::BaseTransition<PlayerStateState, PlayerStateContext>(origin, destination)
	{
	}

	void ClientEnterInit::OnInit()
	{
		mCharacter = GetContext()->GetCharacter();
	}

	bool ClientEnterInit::CanPerformTransition() const
	{
		const auto anyKeyPressed = mCharacter->AnyKeyPressed();
		
		return anyKeyPressed;
	}	
};

#include "EnterGameLoop.h"

#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/GameMode/IPlayerState.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/PlayerStateContext.h"

namespace BRPlayerStateFSM
{
	EnterGameLoop::EnterGameLoop(StatePtr origin, StatePtr destination) :
		core::utils::FSM::BaseTransition<PlayerStateState, PlayerStateContext>(origin, destination)
	{
	}

	void EnterGameLoop::OnInit()
	{
		mPlayerState = GetContext()->GetPlayerState();
	}

	bool EnterGameLoop::CanPerformTransition() const
	{
		const auto character = mPlayerState->GetCharacter();
		return character && character->IsCharacterValid();
	}	
};
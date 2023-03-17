

#include "EnterGameFinish.h"

#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/SinglePlayer/FSM/SinglePlayerContext.h"

namespace BRModeFSM
{
	EnterGameFinish::EnterGameFinish(StatePtr origin, StatePtr destination) :
		core::utils::FSM::BaseTransition<SinglePlayerState, SinglePlayerContext>(origin, destination)
	{
	}

	void EnterGameFinish::OnInit()
	{
		mGameState = GetContext()->GetGameState();
	}

	bool EnterGameFinish::CanPerformTransition() const
	{
		return mGameState->HasMatchEnded();		
	}
};
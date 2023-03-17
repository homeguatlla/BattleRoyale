

#include "EnterGameLoopSP.h"
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/SinglePlayer/FSM/SinglePlayerContext.h"

namespace BRModeFSM
{
	EnterGameLoopSP::EnterGameLoopSP(StatePtr origin, StatePtr destination) :
		core::utils::FSM::BaseTransition<SinglePlayerState, SinglePlayerContext>(origin, destination)
	{
	}

	void EnterGameLoopSP::OnInit()
	{
		mGameState = GetContext()->GetGameState();
	}

	bool EnterGameLoopSP::CanPerformTransition() const
	{
		return true;		
	}
};
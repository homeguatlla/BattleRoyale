
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/Transitions/EnterMatchEnd.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/BattleRoyaleContext.h"

namespace BRModeFSM
{
	EnterMatchEnd::EnterMatchEnd(StatePtr origin, StatePtr destination) :
		core::utils::FSM::BaseTransition<BattleRoyaleState, BattleRoyaleContext>(origin, destination)
	{
	}

	void EnterMatchEnd::OnInit()
	{
		mGameState = GetContext()->GetGameState();
	}

	bool EnterMatchEnd::CanPerformTransition() const
	{
		return mGameState->HasMatchEnded();		
	}
};
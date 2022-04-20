#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/Transitions/EnterSynchronize.h"

#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/BattleRoyaleContext.h"

namespace BRModeFSM
{
	EnterSynchronize::EnterSynchronize(StatePtr origin, StatePtr destination) :
		core::utils::FSM::BaseTransition<BattleRoyaleState, BattleRoyaleContext>(origin, destination)
	{
	}

	void EnterSynchronize::OnInit()
	{
		mGameState = GetContext()->GetGameState();
	}

	bool EnterSynchronize::CanPerformTransition() const
	{
		return mGameState->DidCountdownStart() && mGameState->DidCountdownFinish();
	}
};
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/Transitions/EnterCountdown.h"

#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/BattleRoyaleContext.h"

namespace BRModeFSM
{
	EnterCountdown::EnterCountdown(StatePtr origin, StatePtr destination) :
		core::utils::FSM::BaseTransition<BattleRoyaleState, BattleRoyaleContext>(origin, destination)
	{
	}

	void EnterCountdown::OnInit()
	{
		mGameState = GetContext()->GetGameState();
	}

	bool EnterCountdown::CanPerformTransition() const
	{
		return !mGameState->IsGameReadyToStart() && mGameState->GetNumTeams() > 1;
	}	
};

#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/Transitions/EnterGameLoop.h"

#include "BattleRoyale/core/GameMode/IMultiplayerGameState.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/BattleRoyaleContext.h"

namespace BRModeFSM
{
	EnterGameLoop::EnterGameLoop(StatePtr origin, StatePtr destination) :
		core::utils::FSM::BaseTransition<BattleRoyaleState, BattleRoyaleContext>(origin, destination)
	{
	}

	void EnterGameLoop::OnInit()
	{
		mGameState = Cast<IIMultiplayerGameState>(GetContext()->GetGameState());
	}

	bool EnterGameLoop::CanPerformTransition() const
	{
		return mGameState->AreAllPlayersReplicated();		
	}
};
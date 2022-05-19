
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/Transitions/EnterReset.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/BattleRoyaleContext.h"

namespace BRModeFSM
{
	EnterReset::EnterReset(StatePtr origin, StatePtr destination) :
		core::utils::FSM::BaseTransition<BattleRoyaleState, BattleRoyaleContext>(origin, destination)
	{
	}

	void EnterReset::OnInit()
	{
	}

	bool EnterReset::CanPerformTransition() const
	{
		return GetContext()->IsTimeOver();		
	}
};
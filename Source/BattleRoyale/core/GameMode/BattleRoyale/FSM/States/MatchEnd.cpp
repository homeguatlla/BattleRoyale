#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/MatchEnd.h"

#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/BattleRoyaleContext.h"

namespace BRModeFSM
{
	MatchEnd::MatchEnd()
	{
	}

	void MatchEnd::OnInit()
	{
	}

	void MatchEnd::OnEnter(float deltaTime)
	{
		GetContext()->StartTimer(10);
	}
};
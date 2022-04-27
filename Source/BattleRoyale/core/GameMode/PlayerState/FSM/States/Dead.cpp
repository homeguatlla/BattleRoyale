
#include "Dead.h"

#include "BattleRoyale/core/GameMode/PlayerState/FSM/PlayerStateContext.h"

namespace BRPlayerStateFSM
{
	void Dead::OnInit()
	{
		
	}
	
	void Dead::OnEnter(float deltaTime)
	{
		GetContext()->StartTimer(5);
	}
};
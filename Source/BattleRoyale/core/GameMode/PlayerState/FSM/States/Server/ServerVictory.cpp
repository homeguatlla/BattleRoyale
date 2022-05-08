
#include "ServerVictory.h"

#include "BattleRoyale/core/GameMode/PlayerState/FSM/PlayerStateContext.h"


namespace BRPlayerStateFSM
{
	void ServerVictory::OnInit()
	{
		
	}
	
	void ServerVictory::OnEnter(float deltaTime)
	{
		GetContext()->StartTimer(5);
	}
};
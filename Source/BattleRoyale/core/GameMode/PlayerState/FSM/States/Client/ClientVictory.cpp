
#include "ClientVictory.h"

#include "BattleRoyale/core/GameMode/PlayerState/FSM/PlayerStateContext.h"

namespace BRPlayerStateFSM
{
	void ClientVictory::OnInit()
	{
		
	}
	
	void ClientVictory::OnEnter(float deltaTime)
	{
		GetContext()->StartTimer(5);
	}
};
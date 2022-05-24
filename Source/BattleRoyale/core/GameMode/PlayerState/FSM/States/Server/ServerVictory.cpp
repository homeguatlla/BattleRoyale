
#include "ServerVictory.h"

#include "BattleRoyale/core/GameMode/IPlayerState.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/PlayerStateContext.h"


namespace BRPlayerStateFSM
{
	void ServerVictory::OnInit()
	{
		mPlayerState = GetContext()->GetPlayerState();
	}
	
	void ServerVictory::OnEnter(float deltaTime)
	{
		GetContext()->StartTimer(mPlayerState->GetTimeBetweenEndOfGameAndStatsScreen());
		mPlayerState->ShowVictoryScreen();
	}
};

#include "ClientDead.h"

#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/GameMode/IPlayerState.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/PlayerStateContext.h"


namespace BRPlayerStateFSM
{
	void ClientDead::OnInit()
	{
		mCharacter = GetContext()->GetCharacter();
	}
	
	void ClientDead::OnEnter(float deltaTime)
	{
		mCharacter->DieClient();
		GetContext()->GetPlayerState()->ShowDeathScreen();
	}
};
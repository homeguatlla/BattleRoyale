
#include "ServerGameLoop.h"
#include <functional>

#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/PlayerStateContext.h"

namespace BRPlayerStateFSM
{
	void ServerGameLoop::OnInit()
	{
		mCharacter = GetContext()->GetCharacter();
	}

	void ServerGameLoop::OnEnter(float deltaTime)
	{
		mCharacter->SetInvulnerableServer(true);
		GetContext()->StartTimer(5, std::bind(&ServerGameLoop::OnTimeOver, this));
	}

	void ServerGameLoop::OnTimeOver() const
	{
		mCharacter->SetInvulnerableServer(false);
	}
};
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/Countdown.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/BattleRoyaleContext.h"


namespace BRModeFSM
{
	Countdown::Countdown()
	{
	}

	void Countdown::OnInit()
	{
		mGameState = GetContext()->GetGameState();
	}

	void Countdown::OnEnter(float deltaTime)
	{
		mGameState->StartCountdownServer(GetContext()->GetCountdownTimeToStartGame());
	}

	void Countdown::OnExit(float deltaTime)
	{
		mGameState->StartGameServer();
	}
};
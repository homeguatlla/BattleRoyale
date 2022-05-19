#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/Countdown.h"

#include "BattleRoyale/core/GameMode/BattleRoyale/BattleRoyaleGameState.h"
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
		auto battleRoyaleGameState = Cast<ABattleRoyaleGameState>(mGameState);
		battleRoyaleGameState->StartCountdownServer(GetContext()->GetCountdownTimeToStartGame());
	}

	void Countdown::OnExit(float deltaTime)
	{
	}
};
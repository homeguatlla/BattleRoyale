#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/MatchEnd.h"

#include "BattleRoyale/core/GameMode/BattleRoyale/BattleRoyaleConfigurationInfo.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/BattleRoyaleGameMode.h"
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
		const auto battleRoyaleGameMode = Cast<ABattleRoyaleGameMode>(GetContext()->GetGameMode());		
		GetContext()->StartTimer(battleRoyaleGameMode->GetOnceMatchEndedTimeToResetSessionTime());
	}
};
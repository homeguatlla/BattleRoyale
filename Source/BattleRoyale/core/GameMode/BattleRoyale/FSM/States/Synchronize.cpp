#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/Synchronize.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/BattleRoyaleContext.h"

namespace BRModeFSM
{
	Synchronize::Synchronize()
	{
	}

	void Synchronize::OnEnter(float deltaTime)
	{
		UE_LOG(LogFSM, Log, TEXT("Synchronize::OnEnter"));
	}

	void Synchronize::OnExit(float deltaTime)
	{
		GetContext()->GetGameState()->NotifyGameModeConfigurationInfo(GetContext()->GetGameMode()->GetGameModeConfiguration());
		UE_LOG(LogFSM, Log, TEXT("Synchronize::OnExit"));
	}
};
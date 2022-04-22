#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/Synchronize.h"
#include "BattleRoyale/BattleRoyale.h"

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
		UE_LOG(LogFSM, Log, TEXT("Synchronize::OnExit"));
	}
};
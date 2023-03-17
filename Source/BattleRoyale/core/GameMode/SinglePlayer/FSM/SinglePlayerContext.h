#pragma once
#include "BattleRoyale/core/GameMode/IGameMode.h"
#include "BattleRoyale/core/Utils/FSM/FSMContextBase.h"

namespace BRModeFSM
{
	class SinglePlayerContext : public FSMContextBase
	{
	public:
		SinglePlayerContext(UWorld* world, const IIGameMode* gameMode, IIGameState* gameState);
		~SinglePlayerContext() = default;

		const IIGameMode* GetGameMode() const { return mGameMode; }
		IIGameState* GetGameState() const { return mGameState; }

	private:		
		const IIGameMode* mGameMode;
		IIGameState* mGameState;
	};
};

#pragma once
#include "BattleRoyale/core/GameMode/IGameState.h"

namespace BRModeFSM
{
	class BattleRoyaleContext
	{
	public:
		BattleRoyaleContext(UWorld* world, IIGameState* gameState, float countdownTimeToStartGame);

		~BattleRoyaleContext() = default;

		IIGameState* GetGameState() const { return mGameState; }
		float GetCountdownTimeToStartGame() const { return mCountdownTimeToStartGame; }
		
	private:
		UWorld* mWorld;
		float mCountdownTimeToStartGame;
		IIGameState* mGameState;
	};
};

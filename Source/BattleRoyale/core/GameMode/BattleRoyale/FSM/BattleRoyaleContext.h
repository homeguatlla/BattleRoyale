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
		UWorld* GetWorld() const { return mWorld; }
		
	private:
		UWorld* mWorld;
		IIGameState* mGameState;
		float mCountdownTimeToStartGame;
	};
};

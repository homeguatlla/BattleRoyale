#pragma once
#include "BattleRoyale/core/GameMode/IGameMode.h"
#include "BattleRoyale/core/Utils/FSM/FSMContextBase.h"

namespace BRModeFSM
{
	class BattleRoyaleContext : public FSMContextBase
	{
	public:
		BattleRoyaleContext(UWorld* world, const IIGameMode* gameMode, IIGameState* gameState, float countdownTimeToStartGame);

		~BattleRoyaleContext() = default;

		const IIGameMode* GetGameMode() const { return mGameMode; }
		IIGameState* GetGameState() const { return mGameState; }
		float GetCountdownTimeToStartGame() const { return mCountdownTimeToStartGame; }

	private:		
		const IIGameMode* mGameMode;
		IIGameState* mGameState;
		float mCountdownTimeToStartGame;
	};
};

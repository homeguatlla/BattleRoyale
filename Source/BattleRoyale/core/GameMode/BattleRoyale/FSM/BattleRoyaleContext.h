#pragma once
#include "BattleRoyale/core/GameMode/IGameMode.h"
#include "BattleRoyale/core/Utils/FSM/FSMContextBase.h"

namespace BRModeFSM
{
	class BattleRoyaleContext : public FSMContextBase
	{
	public:
		BattleRoyaleContext(UWorld* world, const IIGameMode* gameMode, IIGameState* gameState, float countdownTimeToStartGame, uint8 numTeamsToStartCountDown);

		~BattleRoyaleContext() = default;

		const IIGameMode* GetGameMode() const { return mGameMode; }
		IIGameState* GetGameState() const { return mGameState; }
		float GetCountdownTimeToStartGame() const { return mCountdownTimeToStartGame; }
		uint8 GetNumTeamsToStartCountDown() const { return mNumTeamsToStartCountDown; }

	private:		
		const IIGameMode* mGameMode;
		IIGameState* mGameState;
		float mCountdownTimeToStartGame;
		uint8 mNumTeamsToStartCountDown;
	};
};

#pragma once
#include <string>
#include <map>

namespace BRModeFSM
{
	enum class BattleRoyaleState
	{
		STATE_INIT = 0,
		STATE_COUNTDOWN = 1,
		STATE_SYNCHRONIZE = 2,
		STATE_GAMELOOP = 3
	};

	static std::map<BattleRoyaleState, std::string> BattleRoyaleStateMap =
	{
		{ BattleRoyaleState::STATE_INIT, std::string("Init") },
		{ BattleRoyaleState::STATE_COUNTDOWN, std::string("Countdown") },
		{ BattleRoyaleState::STATE_SYNCHRONIZE, std::string("Synchronize") },
		{ BattleRoyaleState::STATE_GAMELOOP, std::string("GameLoop") }
	};
};

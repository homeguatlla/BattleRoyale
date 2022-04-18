#pragma once
#include <string>
#include <map>

namespace BRModeFSM
{
	enum class BattleRoyaleState
	{
		STATE_INIT = 0
	};

	static std::map<BattleRoyaleState, std::string> BattleRoyaleStateMap =
	{
		{ BattleRoyaleState::STATE_INIT, std::string("Init") },
	};
};

#pragma once
#include <string>
#include <map>

namespace BRModeFSM
{
	enum class SinglePlayerState
	{
		STATE_INIT = 0,
		STATE_GAMELOOP = 1,
		STATE_MATCHEND = 2,
		STATE_RESTART = 3
	};

	static std::map<SinglePlayerState, std::string> SinglePlayerStateMap =
	{
		{ SinglePlayerState::STATE_INIT, std::string("Init") },
		{ SinglePlayerState::STATE_GAMELOOP, std::string("GameLoop") },
		{ SinglePlayerState::STATE_MATCHEND, std::string("MatchEnd") },
		{ SinglePlayerState::STATE_RESTART, std::string("Restart") }
	};
};

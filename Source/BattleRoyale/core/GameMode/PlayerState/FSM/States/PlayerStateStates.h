#pragma once
#include <string>
#include <map>

namespace BRPlayerStateFSM
{
	enum class PlayerStateState
	{
		STATE_INIT = 0,
		STATE_GAMELOOP = 1,
		STATE_DEAD = 2,
		STATE_STATS = 3
	};

	static std::map<PlayerStateState, std::string> PlayerStateStateMap =
	{
		{ PlayerStateState::STATE_INIT, std::string("Init") },
		{ PlayerStateState::STATE_GAMELOOP, std::string("GameLoop") },
		{ PlayerStateState::STATE_DEAD, std::string("Dead") },
		{ PlayerStateState::STATE_STATS, std::string("Stats") }
	};
};

#pragma once
#include <string>
#include <map>

namespace BRPlayerStateFSM
{
	enum class PlayerStateState
	{
		STATE_INIT = 0,
		STATE_GAMELOOP = 1,
		STATE_DEAD_SERVER = 2,
		STATE_DEAD_CLIENT = 3,
		STATE_VICTORY_SERVER = 4,
		STATE_VICTORY_CLIENT = 5,
		STATE_GAME_OVER_SERVER = 6,
		STATE_GAME_OVER_CLIENT = 7
	};

	static std::map<PlayerStateState, std::string> PlayerStateStateMap =
	{
		{ PlayerStateState::STATE_INIT, std::string("Init") },
		{ PlayerStateState::STATE_GAMELOOP, std::string("GameLoop") },
		{ PlayerStateState::STATE_DEAD_SERVER, std::string("DeadServer") },
		{ PlayerStateState::STATE_DEAD_CLIENT, std::string("DeadClient") },
		{ PlayerStateState::STATE_VICTORY_SERVER, std::string("VictoryServer") },
		{ PlayerStateState::STATE_VICTORY_CLIENT, std::string("VictoryClient") },
		{ PlayerStateState::STATE_GAME_OVER_SERVER, std::string("GameOverServer") },
		{ PlayerStateState::STATE_GAME_OVER_CLIENT, std::string("GameOVerClient") }
	};
};

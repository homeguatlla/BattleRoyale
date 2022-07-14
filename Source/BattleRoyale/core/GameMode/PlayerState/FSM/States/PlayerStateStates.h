#pragma once
#include <string>
#include <map>

namespace BRPlayerStateFSM
{
	enum class PlayerStateState
	{
		STATE_INIT = 0,
		STATE_GAMELOOP_SERVER = 1,
		STATE_GAMELOOP_CLIENT = 2,
		STATE_DEAD_SERVER = 3,
		STATE_DEAD_CLIENT = 4,
		STATE_VICTORY_SERVER = 5,
		STATE_VICTORY_CLIENT = 6,
		STATE_GAME_OVER_SERVER = 7,
		STATE_GAME_OVER_CLIENT = 8,
		STATE_RESTART_CLIENT = 9,
		STATE_RESTART_SERVER = 10
	};

	static std::map<PlayerStateState, std::string> PlayerStateStateMap =
	{
		{ PlayerStateState::STATE_INIT, std::string("Init") },
		{ PlayerStateState::STATE_GAMELOOP_SERVER, std::string("GameLoopServer") },
		{ PlayerStateState::STATE_GAMELOOP_CLIENT, std::string("GameLoopClient") },
		{ PlayerStateState::STATE_DEAD_SERVER, std::string("DeadServer") },
		{ PlayerStateState::STATE_DEAD_CLIENT, std::string("DeadClient") },
		{ PlayerStateState::STATE_VICTORY_SERVER, std::string("VictoryServer") },
		{ PlayerStateState::STATE_VICTORY_CLIENT, std::string("VictoryClient") },
		{ PlayerStateState::STATE_GAME_OVER_SERVER, std::string("GameOverServer") },
		{ PlayerStateState::STATE_GAME_OVER_CLIENT, std::string("GameOVerClient") },
		{ PlayerStateState::STATE_RESTART_CLIENT, std::string("RestartClient") },
		{ PlayerStateState::STATE_RESTART_SERVER, std::string("RestartServer") }
	};
};

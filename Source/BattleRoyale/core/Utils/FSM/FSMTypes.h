#pragma once
#include <string>
#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/PlayerStateStates.h"

enum class FSMType
{
	BATTLEROYALE_GAMEMODE = 0,
	COOPERATIVE_GAMEMODE = 1,
	SINGLEPLAYER_GAMEMODE = 2,
	PLAYER_STATE_SERVER = 3,
	PLAYER_STATE_CLIENT = 4
};

template<typename TStateID>
static std::string GetStateName(TStateID stateId)
{
	if(typeid(TStateID) == typeid(BRModeFSM::BattleRoyaleState))
	{
		return BRModeFSM::BattleRoyaleStateMap[static_cast<BRModeFSM::BattleRoyaleState>(stateId)];
	}
	else if(typeid(TStateID) == typeid(BRPlayerStateFSM::PlayerStateState))
	{
		return BRPlayerStateFSM::PlayerStateStateMap[static_cast<BRPlayerStateFSM::PlayerStateState>(stateId)];
	}
	else return "NotFound";
}
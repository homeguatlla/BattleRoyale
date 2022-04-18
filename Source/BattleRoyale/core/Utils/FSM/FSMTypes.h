#pragma once
#include <string>

enum class FSMType
{
	BATTLEROYALE_GAMEMODE = 0,
	
	COOPERATIVE_GAMEMODE = 10
};

template<typename TStateID>
static std::string GetStateName(TStateID stateId)
{
	if(typeid(TStateID) == typeid(TLN::Chicken::ChickenState))
	{
		return TLN::Chicken::ChickenStateMap[static_cast<TLN::Chicken::ChickenState>(stateId)];
	}
	else if(typeid(TStateID) == typeid(TLN::CharacterState))
	{
		return TLN::CharacterStateMap[static_cast<TLN::CharacterState>(stateId)];
	}
	else return "NotFound";
}
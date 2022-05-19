#pragma once
#include "FSMTypes.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/BattleRoyaleStates.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/BattleRoyaleContext.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/PlayerStateContext.h"
#include "BattleRoyale/core/Utils/FSM/fsm/StatesMachine.h"


namespace BattleRoyale
{
	class StatesMachineFactory
	{
		using GameModeFSM = core::utils::FSM::StatesMachine<BRModeFSM::BattleRoyaleState, BRModeFSM::BattleRoyaleContext>;
		using PlayerStateFSM = core::utils::FSM::StatesMachine<BRPlayerStateFSM::PlayerStateState, BRPlayerStateFSM::PlayerStateContext>;
		
	public:
		StatesMachineFactory() = default;
		~StatesMachineFactory() = default;

		std::unique_ptr<GameModeFSM> CreateModeFSM(FSMType type, std::shared_ptr<BRModeFSM::BattleRoyaleContext> context);
		std::unique_ptr<PlayerStateFSM> CreatePlayerStateFSM(FSMType type, std::shared_ptr<BRPlayerStateFSM::PlayerStateContext> context);
	};
}
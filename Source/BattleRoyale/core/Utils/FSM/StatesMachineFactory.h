#pragma once
#include "FSMTypes.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/BattleRoyaleStates.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/BattleRoyaleContext.h"
#include "BattleRoyale/core/Utils/FSM/fsm/StatesMachine.h"


namespace BattleRoyale
{
	class StatesMachineFactory
	{
		using BRGameModeFSM = core::utils::FSM::StatesMachine<BRModeFSM::BattleRoyaleState, BRModeFSM::BattleRoyaleContext>;
		//using ChickenStatesMachine = core::utils::FSM::StatesMachine<TLN::Chicken::ChickenState, TLN::Chicken::ChickenContext>;
	public:
		StatesMachineFactory() = default;
		~StatesMachineFactory() = default;

		std::unique_ptr<BRGameModeFSM> CreateBattleRoyaleModeFSM(FSMType type, std::shared_ptr<BRModeFSM::BattleRoyaleContext> context);
		//std::unique_ptr<ChickenStatesMachine> CreateChicken(FSMType type, std::shared_ptr<TLN::Chicken::ChickenContext> context);
	};
}
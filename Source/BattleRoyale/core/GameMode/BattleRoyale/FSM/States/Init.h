#pragma once
#include "BattleRoyaleStates.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/BattleRoyaleContext.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseState.h"


namespace BRModeFSM
{
	class Init : public core::utils::FSM::BaseState<BattleRoyaleState, BattleRoyaleContext>
	{
	public:
		Init();
		virtual ~Init() override = default;

		virtual BattleRoyaleState GetID() const override { return BattleRoyaleState::STATE_INIT; }
		
		virtual void OnInit() override;
	};
};

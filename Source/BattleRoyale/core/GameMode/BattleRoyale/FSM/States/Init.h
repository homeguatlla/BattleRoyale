#pragma once
#include "BattleRoyaleStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseState.h"


namespace BRModeFSM
{
	class BattleRoyaleContext;

	class Init : public core::utils::FSM::BaseState<BattleRoyaleState, BattleRoyaleContext>
	{
	public:
		Init();
		virtual ~Init() = default;

		BattleRoyaleState GetID() const override { return BattleRoyaleState::STATE_INIT; }
		
		void OnInit() override;
	};
};

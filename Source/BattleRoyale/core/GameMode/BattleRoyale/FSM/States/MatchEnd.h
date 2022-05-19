#pragma once
#include "BattleRoyaleStates.h"
#include "BattleRoyale/core/GameMode/IGameMode.h"
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/GameRules/GameRules.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseState.h"


namespace BRModeFSM
{
	class BattleRoyaleContext;

	class MatchEnd : public core::utils::FSM::BaseState<BattleRoyaleState, BattleRoyaleContext>
	{
	public:
		
		MatchEnd();
		virtual ~MatchEnd() = default;

		BattleRoyaleState GetID() const override { return BattleRoyaleState::STATE_MATCHEND; }
		
		virtual void OnInit() override;
		virtual void OnEnter(float deltaTime) override;
	};
};

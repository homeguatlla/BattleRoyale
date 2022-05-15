#pragma once
#include "BattleRoyaleStates.h"
#include "BattleRoyale/core/GameMode/IGameMode.h"
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/GameRules/GameRules.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseState.h"


namespace BRModeFSM
{
	class BattleRoyaleContext;

	class Reset : public core::utils::FSM::BaseState<BattleRoyaleState, BattleRoyaleContext>
	{
	public:
		
		Reset();
		virtual ~Reset() = default;

		BattleRoyaleState GetID() const override { return BattleRoyaleState::STATE_RESET; }
		
		virtual void OnInit() override;
		virtual void OnEnter(float deltaTime) override;

	private:
		const IIGameMode* mGameMode;
	};
};

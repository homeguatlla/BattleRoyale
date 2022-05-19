#pragma once

#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/PlayerStateStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseState.h"

class IIPlayerState;

namespace BRPlayerStateFSM
{
	class PlayerStateContext;

	class ClientGameOver : public core::utils::FSM::BaseState<PlayerStateState, PlayerStateContext>
	{
	public:
		
		ClientGameOver() = default;
		virtual ~ClientGameOver() = default;

		PlayerStateState GetID() const override { return PlayerStateState::STATE_GAME_OVER_CLIENT; }
		
		virtual void OnInit() override;
		virtual void OnEnter(float deltaTime) override;
		
	private:
		IIPlayerState* mPlayerState;
	};
};

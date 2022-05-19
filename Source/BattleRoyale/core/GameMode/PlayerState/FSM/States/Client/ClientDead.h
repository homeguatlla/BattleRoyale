#pragma once

#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/PlayerStateStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseState.h"


class IICharacter;

namespace BRPlayerStateFSM
{
	class PlayerStateContext;

	class ClientDead : public core::utils::FSM::BaseState<PlayerStateState, PlayerStateContext>
	{
	public:
		
		ClientDead() = default;
		virtual ~ClientDead() = default;

		PlayerStateState GetID() const override { return PlayerStateState::STATE_DEAD_CLIENT; }
		
		virtual void OnInit() override;
		virtual void OnEnter(float deltaTime) override;
		
	private:
		IICharacter* mCharacter;
	};
};

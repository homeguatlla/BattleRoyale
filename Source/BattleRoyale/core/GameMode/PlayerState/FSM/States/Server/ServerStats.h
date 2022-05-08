#pragma once
#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/PlayerStateStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseState.h"

class IIPlayerState;

namespace BRPlayerStateFSM
{
	class PlayerStateContext;

	class ServerStats : public core::utils::FSM::BaseState<PlayerStateState, PlayerStateContext>
	{
	public:
		
		ServerStats() = default;
		virtual ~ServerStats() = default;

		PlayerStateState GetID() const override { return PlayerStateState::STATE_STATS_SERVER; }
		
		virtual void OnInit() override;
		virtual void OnEnter(float deltaTime) override;
		
	private:
		IIPlayerState* mPlayerState;
	};
};

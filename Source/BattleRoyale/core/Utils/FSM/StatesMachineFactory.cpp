#include "StatesMachineFactory.h"

//FSM BattleRoyale States
#include <BattleRoyale/core/GameMode/BattleRoyale/FSM/States/Init.h>

//FSM BattleRoyale Transitions
#include <BattleRoyale/core/GameMode/BattleRoyale/FSM/Transitions/EnterCountdown.h>

#include "StatesMachineBuilder.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/Countdown.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/GameLoop.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/Synchronize.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/Transitions/EnterGameLoop.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/Transitions/EnterSynchronize.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/Dead.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/PSGameLoop.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/Init.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/Stats.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/Transitions/EnterDead.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/Transitions/EnterGameLoop.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/Transitions/EnterStats.h"

namespace BattleRoyale
{
	std::unique_ptr<StatesMachineFactory::GameModeFSM> StatesMachineFactory::CreateModeFSM(
		FSMType type, 
		std::shared_ptr<BRModeFSM::BattleRoyaleContext> context)
	{
		StatesMachineBuilder<BRModeFSM::BattleRoyaleState, BRModeFSM::BattleRoyaleContext> builder;

		switch(type)
		{
			case FSMType::BATTLEROYALE_GAMEMODE:
				{
					const auto init = std::make_shared<BRModeFSM::Init>();
					const auto countdown = std::make_shared<BRModeFSM::Countdown>();
					const auto synchronize = std::make_shared<BRModeFSM::Synchronize>();
					const auto gameLoop = std::make_shared<BRModeFSM::GameLoop>();
					
					return builder.WithState(init)
								  .WithState(countdown)
								  .WithState(synchronize)
								  .WithState(gameLoop)
	                              .WithTransition(std::make_unique<BRModeFSM::EnterCountdown>(init, countdown))
	                              .WithTransition(std::make_unique<BRModeFSM::EnterSynchronize>(countdown, synchronize))
	                              .WithTransition(std::make_unique<BRModeFSM::EnterGameLoop>(synchronize, gameLoop))
	                              .WithInitialState(init->GetID())
	                              .Build(context);
				}
			
			default:
				checkf(false, TEXT("States Machine type %d not defined"), type);
				return {};
		}	
	}
	std::unique_ptr<StatesMachineFactory::PlayerStateFSM> StatesMachineFactory::CreatePlayerStateFSM(
			FSMType type, 
			std::shared_ptr<BRPlayerStateFSM::PlayerStateContext> context)
	{
		StatesMachineBuilder<BRPlayerStateFSM::PlayerStateState, BRPlayerStateFSM::PlayerStateContext> builder;

		switch(type)
		{
		case FSMType::PLAYER_STATE:
			{
				const auto init = std::make_shared<BRPlayerStateFSM::Init>();
				const auto gameLoop = std::make_shared<BRPlayerStateFSM::PSGameLoop>();
				const auto dead = std::make_shared<BRPlayerStateFSM::Dead>();
				const auto stats = std::make_shared<BRPlayerStateFSM::Stats>();
					
				return builder.WithState(init)
							  .WithState(gameLoop)
							  .WithState(dead)
							  .WithState(stats)
							  .WithTransition(std::make_unique<BRPlayerStateFSM::EnterGameLoop>(init, gameLoop))
							  .WithTransition(std::make_unique<BRPlayerStateFSM::EnterDead>(gameLoop, dead))
							  .WithTransition(std::make_unique<BRPlayerStateFSM::EnterStats>(dead, stats))
							  .WithInitialState(init->GetID())
							  .Build(context);
			}
			
		default:
			checkf(false, TEXT("States Machine type %d not defined"), type);
			return {};
		}	
	}
}

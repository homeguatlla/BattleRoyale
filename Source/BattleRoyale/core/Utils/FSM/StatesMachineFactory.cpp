#include "StatesMachineFactory.h"

//FSM BattleRoyale States
#include <BattleRoyale/core/GameMode/BattleRoyale/FSM/States/Init.h>

//FSM BattleRoyale Transitions
#include <BattleRoyale/core/GameMode/BattleRoyale/FSM/Transitions/EnterCountdown.h>

#include "StatesMachineBuilder.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/Countdown.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/Synchronize.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/Transitions/EnterGameLoop.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/Transitions/EnterSynchronize.h"

namespace BattleRoyale
{
	std::unique_ptr<StatesMachineFactory::BRGameModeFSM> StatesMachineFactory::CreateBattleRoyaleModeFSM(
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
					const auto gameLoop = std::make_shared<BRModeFSM::Synchronize>();
					
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
/*
	std::unique_ptr<StatesMachineFactory::ChickenStatesMachine> StatesMachineFactory::CreateChicken(FSMType type, std::shared_ptr<TLN::Chicken::ChickenContext> context)
	{
		StatesMachineBuilder<TLN::Chicken::ChickenState, TLN::Chicken::ChickenContext> builder;

		switch(type)
		{
		case FSMType::CHICKEN_MOVEMENT:
			{
				auto idle = std::make_shared<TLN::Chicken::Idle>();
				auto walk = std::make_shared<TLN::Chicken::Walk>();
				
				return builder.WithState(idle)
                              .WithState(walk)
                              .WithTransition(std::make_unique<TLN::Chicken::EnterWalk>(idle, walk))
                              .WithTransition(std::make_unique<TLN::Chicken::EnterIdle>(walk, idle))
                              .WithInitialState(idle->GetID())
                              .Build(context);
			}
		case FSMType::CHICKEN_STATE:
			{
				auto idle = std::make_shared<TLN::Chicken::IdleState>();
				auto explore = std::make_shared<TLN::Chicken::Explore>();
				auto eat = std::make_shared<TLN::Chicken::Eat>();
				
				return builder.WithState(idle)
                              .WithState(explore)
                              .WithState(eat)
                              .WithTransition(std::make_unique<TLN::Chicken::EnterExplore>(idle, explore))
                              .WithTransition(std::make_unique<TLN::Chicken::LeaveExplore>(explore, idle))
                              .WithTransition(std::make_unique<TLN::Chicken::EnterEat>(idle, eat))
                              .WithTransition(std::make_unique<TLN::Chicken::LeaveEat>(eat, idle))
                              .WithInitialState(idle->GetID())
                              .Build(context);
			}
		default:
			checkf(false, TEXT("States Machine type %d not defined"), type);
			return {};
		}	
	}*/
}

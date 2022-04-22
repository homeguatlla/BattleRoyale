#pragma once

#include "IState.h"
#include "ITransition.h"

#include <vector>
#include <map>
#include <memory>
#include <assert.h>

namespace core {
	namespace utils {
		namespace FSM
		{
			template<typename TStateID, class TContext>
			class StatesMachine
			{
				using StatePtr = std::shared_ptr<core::utils::FSM::IState<TStateID, TContext>>;
				using TransitionPtr = std::shared_ptr<ITransition<TStateID, TContext>>;
				using ContextPtr = std::shared_ptr<TContext>;

			public:
				explicit StatesMachine(ContextPtr context);
				virtual ~StatesMachine();

				void SetInitialState(const TStateID state);
				void ForceState(TStateID state, float deltaTime);

				StatePtr GetCurrentState() const { return currentState; }

				void Reload();
				void Update(float deltaTime);

				void AddState(StatePtr state);
				void AddTransition(std::unique_ptr<ITransition<TStateID, TContext>> transition);
				void AddFromAnyTransition(std::unique_ptr<ITransition<TStateID, TContext>> transition);

				ContextPtr GetContext() const { return context; }

			private:
				bool ExistState(TStateID state) const;
				void ChangeState(StatePtr state, float deltaTime);
				bool PerformTransitions(float deltaTime);
				bool PerformFromAnyTransitions(float deltaTime);
				bool PerformTransitions(std::vector<TransitionPtr>& transitionsRef, float deltaTime);

			private:
				typedef struct StateInformation
				{
					StateInformation() = default;
					StateInformation(StatePtr state) : state{ state } {}

					StatePtr state;
					std::vector<TransitionPtr> transitions;
				} StateInformation;

				StatePtr initialState;
				StatePtr currentState;
				std::map<TStateID, StateInformation> transitions;
				std::vector<TransitionPtr> fromAnyTransitions;
				ContextPtr context;
			};

			template<typename TStateID, class TContext>
			StatesMachine<TStateID, TContext>::StatesMachine(ContextPtr context) :
				currentState{ nullptr },
				context{ std::move(context) }
			{
			}

			template<typename TStateID, class TContext>
			StatesMachine<TStateID, TContext>::~StatesMachine()
			{
				transitions.clear();
			}

			template<typename TStateID, class TContext>
			void StatesMachine<TStateID, TContext>::SetInitialState(const TStateID state)
			{
				auto existState = ExistState(state);
				assert(existState);

				if (existState)
				{
					initialState = transitions[state].state;
				}
			}

			template<typename TStateID, class TContext>
			void StatesMachine<TStateID, TContext>::Reload()
			{
				context->Release();

				for (auto it = transitions.begin(); it != transitions.end(); ++it)
				{
					it->second.state->OnReload();
				}
			}

			template<typename TStateID, class TContext>
			void StatesMachine<TStateID, TContext>::Update(float deltaTime)
			{
				assert(initialState != nullptr);

				if (currentState == nullptr)
				{
					currentState = initialState;
					currentState->OnEnter(deltaTime);
				}

				currentState->OnUpdate(deltaTime);

				bool performed = PerformFromAnyTransitions(deltaTime);
				if (!performed)
				{
					PerformTransitions(deltaTime);
				}
			}

			template<typename TStateID, class TContext>
			bool StatesMachine<TStateID, TContext>::PerformTransitions(std::vector<TransitionPtr>& transitionsRef, float deltaTime)
			{
				for (auto const& transition : transitionsRef)
				{
					//TODO @DG optimize this method because of miss cache in a Tick method.
					if (transition->CanPerformTransition())
					{
						ChangeState(transition->GetDestinationState(), deltaTime);
						return true;
					}
				}
				return false;
			}

			template<typename TStateID, class TContext>
			bool StatesMachine<TStateID, TContext>::PerformTransitions(float deltaTime)
			{
				auto stateTransitions = transitions[currentState->GetID()];

				return PerformTransitions(stateTransitions.transitions, deltaTime);
			}

			template<typename TStateID, class TContext>
			bool StatesMachine<TStateID, TContext>::PerformFromAnyTransitions(float deltaTime)
			{
				return PerformTransitions(fromAnyTransitions, deltaTime);
			}

			template<typename TStateID, class TContext>
			void StatesMachine<TStateID, TContext>::ForceState(TStateID state, float deltaTime)
			{
				bool existState = ExistState(state);
				assert(existState);

				if (existState)
				{
					ChangeState(transitions[state].state, deltaTime);
				}
			}

			template<typename TStateID, class TContext>
			void StatesMachine<TStateID, TContext>::AddState(StatePtr state)
			{
				assert(state != nullptr);

				if (!ExistState(state->GetID()))
				{
					state->Init(context);
					transitions[state->GetID()] = StateInformation(state);
				}
			}

			template<typename TStateID, class TContext>
			void StatesMachine<TStateID, TContext>::AddTransition(std::unique_ptr<ITransition<TStateID, TContext>> transition)
			{
				auto origin = transition->GetOriginState();
				auto destination = transition->GetDestinationState();

				assert(origin != nullptr);
				assert(destination != nullptr);


				bool existStateOrigin = ExistState(origin->GetID());
				bool existStateDestination = ExistState(destination->GetID());

				assert(existStateOrigin);
				assert(existStateDestination);

				if (existStateOrigin && existStateDestination)
				{
					transition->Init(context);
					transitions[origin->GetID()].transitions.push_back(std::move(transition));
				}
			}

			template<typename TStateID, class TContext>
			void StatesMachine<TStateID, TContext>::AddFromAnyTransition(std::unique_ptr<ITransition<TStateID, TContext>> transition)
			{
				auto origin = transition->GetOriginState();
				auto destination = transition->GetDestinationState();

				assert(origin == nullptr);
				assert(destination != nullptr);

				bool existStateDestination = ExistState(destination->GetID());
				assert(existStateDestination);

				if (existStateDestination)
				{
					transition->Init(context);
					fromAnyTransitions.push_back(std::move(transition));
				}
			}

			template<typename TStateID, class TContext>
			bool StatesMachine<TStateID, TContext>::ExistState(TStateID state) const
			{
				bool existState = transitions.find(state) != transitions.end();

				return existState;
			}

			template<typename TStateID, class TContext>
			void StatesMachine<TStateID, TContext>::ChangeState(StatePtr state, float deltaTime)
			{
				if (currentState != nullptr)
				{
					currentState->OnExit(deltaTime);
				}
				currentState = state;
				currentState->OnEnter(deltaTime);
			}
		}
	}
};

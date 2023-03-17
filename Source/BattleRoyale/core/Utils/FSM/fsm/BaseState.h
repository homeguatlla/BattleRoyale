#pragma once
#include "IState.h"

namespace core {
	namespace utils {
		namespace FSM
		{
			template<typename TStateID, class TContext>
			class BaseState : public core::utils::FSM::IState<TStateID, TContext>
			{
			public:
				using ContextPtr = std::shared_ptr<TContext>;
					
				virtual void Init(ContextPtr contextRef) override final
				{
					context = contextRef;
					OnInit();
				}

				virtual void OnInit() override {}
				virtual void OnEnter(float deltaTime) override {}
				virtual void OnExit(float deltaTime) override {}
				virtual void OnReload() override {}
				virtual void OnUpdate(float /*deltaTime*/) override {}

			protected:
				virtual ContextPtr GetContext() const override final
				{
					return context;
				}

			private:
				ContextPtr context;
			};
		}
	}
};

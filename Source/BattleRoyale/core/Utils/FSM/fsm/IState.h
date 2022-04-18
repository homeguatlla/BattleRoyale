#pragma once
#include <memory>

namespace core {
	namespace utils {
		namespace FSM
		{
			template<typename TStateID, class TContext>
			class StatesMachine;

			template<typename TStateID, class TContext>
			class IState
			{
			public:
				using ContextPtr = std::shared_ptr<TContext>;
				virtual ~IState() = default;
				virtual TStateID GetID() const = 0;
				virtual ContextPtr GetContext() const = 0;
				virtual void Init(ContextPtr contextRef) = 0;

			private:
				//The reason of this friend classes is because we only want StatesMachine and BaseTransition call this methods.
				friend class StatesMachine<TStateID, TContext>;

				virtual void OnInit() = 0;
				virtual void OnEnter(float deltaTime) = 0;
				virtual void OnExit(float deltaTime) = 0;
				virtual void OnReload() = 0;
				virtual void OnUpdate(float deltaTime) = 0;
			};
		}
	}
};
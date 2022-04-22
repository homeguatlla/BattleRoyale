#pragma once
#include "ITransition.h"

namespace core {
	namespace utils {
		namespace FSM
		{
			template<typename TStateID, class TContext>
			class BaseTransition : public ITransition<TStateID, TContext>
			{
			public:
				using ContextPtr = std::shared_ptr<TContext>;
				using StatePtr = std::shared_ptr<core::utils::FSM::IState<TStateID, TContext>>;

				BaseTransition(StatePtr origin, StatePtr destination) :
					origin{ origin },
					destination{ destination }
				{
					context = destination->GetContext();
				}

				BaseTransition(StatePtr destination) : BaseTransition(nullptr, destination)
				{
				}

				virtual ~BaseTransition() = default;
				StatePtr GetOriginState() const override final { return origin; }
				StatePtr GetDestinationState() const override final { return destination; }
				ContextPtr GetContext() const override final { return context; }
				void OnInit() override {};

			private:
				void Init(ContextPtr contextRef) override final
				{
					context = contextRef;
					OnInit();
				}

			private:
				StatePtr origin;
				StatePtr destination;
				ContextPtr context;
			};
		}
	}
};
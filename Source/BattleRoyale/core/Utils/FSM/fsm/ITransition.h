#pragma once
#include <memory>
#include "IState.h"

namespace core {
	namespace utils {
		namespace FSM
		{
			template<typename TStateID, class TContext>
			class ITransition
			{
			public:
				using StatePtr = std::shared_ptr <core::utils::FSM::IState<TStateID, TContext >> ;
				using ContextPtr = std::shared_ptr<TContext>;

				virtual ~ITransition() = default;

				virtual bool CanPerformTransition() const = 0;
				virtual StatePtr GetOriginState() const = 0;
				virtual StatePtr GetDestinationState() const = 0;
				virtual ContextPtr GetContext() const = 0;
				virtual void Init(ContextPtr contextRef) = 0;
				virtual void OnInit() = 0;
			};
		}
	}
};
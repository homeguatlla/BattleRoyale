// Fill out your copyright notice in the Description page of Project Settings.


#include "InputMappingContextWithBindings.h"

#include "InputActionBindings.h"
#include "InputMappingContext.h"
#include "BattleRoyale/core/Character/Components/EnhancedInputComponentBase.h"

void UInputMappingContextWithBindings::BindInputs(UEnhancedInputComponentBase* enhancedInputComponent, UObject* owner)
{
	const auto contextMappings = Context->GetMappings();
	for(const auto inputActionMapping : contextMappings)
	{
		const auto inputAction = inputActionMapping.Action;
		for(const auto inputActionBinding : InputActionBindings)
		{
			if(inputActionBinding->GetInputActionName() == inputAction->GetName())
			{
				inputActionBinding->PerformActionForEachEvent([enhancedInputComponent, inputAction, owner](const FInputBindingData& event)
				{
					enhancedInputComponent->BindAction(inputAction.Get(), event.TriggerEvent, owner, event.FunctionName); 
				});
			}
		}
	}
}

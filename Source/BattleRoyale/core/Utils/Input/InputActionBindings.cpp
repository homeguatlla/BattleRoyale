// Fill out your copyright notice in the Description page of Project Settings.


#include "InputActionBindings.h"

void UInputActionBindings::PerformActionForEachEvent(
	const std::function<void(const FInputBindingData& eventData)>& action)
{
	for(const auto event : Events)
	{
		action(event);
	}
}

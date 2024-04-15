// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputComponentBase.generated.h"


class UInputMappingContextWithBindings;

UCLASS(Blueprintable)
class BATTLEROYALE_API UEnhancedInputComponentBase : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	UEnhancedInputComponentBase();

	void Initialize(UInputMappingContextWithBindings* defaultMappingContext, const APlayerController* playerController);
	void BindInputs(UObject* owner);
	
private:
	
	UPROPERTY()
	UInputMappingContextWithBindings* mDefaultMappingContext = nullptr;
};

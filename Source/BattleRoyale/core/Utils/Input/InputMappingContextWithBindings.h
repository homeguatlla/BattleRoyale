// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InputMappingContextWithBindings.generated.h"

class UInputActionBindings;
class UInputMappingContext;
class UEnhancedInputComponentBase;

/**
 * 
 */
UCLASS(BlueprintType)
class BATTLEROYALE_API UInputMappingContextWithBindings : public UDataAsset
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TArray<UInputActionBindings*> InputActionBindings;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* Context;

public:
	UInputMappingContext* GetContext() const { return Context; }
	void BindInputs(UEnhancedInputComponentBase* enhancedInputComponent, UObject* owner);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "UObject/Object.h"
#include "InputActionBindings.generated.h"


USTRUCT(BlueprintType)
struct FInputBindingData
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, Category = "Binding")
	ETriggerEvent TriggerEvent;
	UPROPERTY(EditDefaultsOnly, Category = "Binding")
	FName FunctionName;
};
/**
 * 
 */
UCLASS(BlueprintType)
class BATTLEROYALE_API UInputActionBindings : public UDataAsset
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "InputAction")
	FString Name;
	UPROPERTY(EditDefaultsOnly, Category = "InputAction")
	TArray<FInputBindingData> Events;

public:
	FString GetInputActionName() const { return Name; }
	void PerformActionForEachEvent(const std::function<void (const FInputBindingData& eventData)>& action);
};

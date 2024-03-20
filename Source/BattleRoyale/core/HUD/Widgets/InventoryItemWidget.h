// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UInventoryItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = OnMouseEnter))
	void BP_OnMouseEnter();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = OnMouseLeave))
	void BP_OnMouseLeave();
};

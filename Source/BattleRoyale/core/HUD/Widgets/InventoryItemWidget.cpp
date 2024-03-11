// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemWidget.h"
#include "InventoryItemOverlay.h"


FReply UInventoryItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry,
                                                     const FPointerEvent& InMouseEvent)
{
	if(!InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		return FReply::Unhandled();
	}

	if(const auto inventoryItemOverlay = Cast<UInventoryItemOverlay>(GetParent()))
	{
		inventoryItemOverlay->DropItem();
		return FReply::Handled();
	}
	
	return FReply::Unhandled();
}
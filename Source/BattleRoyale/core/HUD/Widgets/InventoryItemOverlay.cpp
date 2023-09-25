// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemOverlay.h"
#include "IInventoryItemOverlay.h"

void UInventoryItemOverlay::SetItem(int value, const TSubclassOf<UUserWidget>& widget)
{
	if(!widget->IsValidLowLevel())
	{
		return;
	}
	const auto itemWidget = CreateWidget<UUserWidget>(this, widget);
	if(!itemWidget)
	{
		return;
	}
	
	CurrentWidgetClass = widget;
	RemoveChildAt(0);
	AddChild(itemWidget);

	if(itemWidget->Implements<UIInventoryItemOverlay>())
	{
		const auto inventoryItemOverlayWidget = Cast<IIInventoryItemOverlay>(itemWidget);
		inventoryItemOverlayWidget->Execute_SetValue(itemWidget, value);
	}
	
	mIsEmpty = false;
}

void UInventoryItemOverlay::RemoveItem()
{
	if(mIsEmpty)
	{
		return;
	}
	const auto itemWidget = CreateWidget<UUserWidget>(this, DefaultWidgetClass);
	if(!itemWidget)
	{
		return;
	}
	CurrentWidgetClass = DefaultWidgetClass;
	RemoveChildAt(0);
	AddChild(itemWidget);
	mIsEmpty = true;
}

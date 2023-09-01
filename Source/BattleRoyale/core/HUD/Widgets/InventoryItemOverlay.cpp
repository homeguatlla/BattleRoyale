// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemOverlay.h"

void UInventoryItemOverlay::SetItem(const TSubclassOf<UUserWidget>& widget)
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
	RemoveChild(GetChildAt(0));
    	
	AddChild(itemWidget);
	mIsEmpty = false;
}

void UInventoryItemOverlay::RemoveItem()
{
	const auto itemWidget = CreateWidget<UUserWidget>(this, DefaultWidgetClass);
	if(!itemWidget)
	{
		return;
	}
	CurrentWidgetClass = DefaultWidgetClass;
	RemoveChild(GetChildAt(0));
	AddChild(itemWidget);
	mIsEmpty = true;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillOverlay.h"

void USkillOverlay::SetItem(int cooldownTime, const TSubclassOf<UUserWidget>& widget)
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
	mCooldownTime = cooldownTime;
	CurrentWidgetClass = widget;
	RemoveChildAt(0);
	AddChild(itemWidget);
}

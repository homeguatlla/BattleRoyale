// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillOverlay.h"

#include "SkillWidget.h"

void USkillOverlay::SetItem(float cooldownTime, const TSubclassOf<USkillWidget>& widget)
{
	if(!widget->IsValidLowLevel())
	{
		return;
	}
	mWidget = CreateWidget<USkillWidget>(this, widget);
	if(!mWidget)
	{
		return;
	}
	mCooldownTime = cooldownTime;
	CurrentWidgetClass = widget;
	RemoveChildAt(0);
	AddChild(mWidget);
}

void USkillOverlay::StartCooldown()
{
	check(mWidget);
	
	mWidget->BP_OnStartCooldown(mCooldownTime);
}
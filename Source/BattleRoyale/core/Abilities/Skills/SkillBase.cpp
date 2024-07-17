// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillBase.h"

USkillBase::USkillBase()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	//CooldownGameplayEffectClass = UCooldownGameplayEffect::StaticClass();
}



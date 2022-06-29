
#pragma once
#include "GameplayTagContainer.h"

const FGameplayTag TAG_ABILITY_HURT = FGameplayTag::RequestGameplayTag(FName("Ability.Hurt"));
const FGameplayTag TAG_ABILITY_JUMP = FGameplayTag::RequestGameplayTag(FName("Ability.Jump"));
const FGameplayTag TAG_ABILITY_SPRINT = FGameplayTag::RequestGameplayTag(FName("Ability.Sprint"));
const FGameplayTag TAG_ABILITY_SHOOT = FGameplayTag::RequestGameplayTag(FName("Ability.Shoot"));
const FGameplayTag TAG_ABILITY_SHOOT_PROJECTILE = FGameplayTag::RequestGameplayTag(FName("Ability.Shoot.Projectile"));

const FGameplayTag TAG_STATE_CAN_BE_HURT = FGameplayTag::RequestGameplayTag(FName("State.CanBeHurt"));


const FGameplayTag TAG_GAMEPLAYCUE_JUMP = FGameplayTag::RequestGameplayTag(FName("GameplayCue.Jump"));
const FGameplayTag TAG_GAMEPLAYCUE_SPRINT = FGameplayTag::RequestGameplayTag(FName("GameplayCue.Sprint"));

const FGameplayTag TAG_EVENT_MONTAGE_SHOOT = FGameplayTag::RequestGameplayTag(FName("Event.Montage.Shoot"));
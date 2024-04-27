
#pragma once

const FName TAG_ABILITY_HURT("Ability.Hurt");
const FName TAG_ABILITY_JUMP("Ability.Jump");
const FName TAG_ABILITY_SPRINT("Ability.Sprint");
const FName TAG_ABILITY_CROUCH("Ability.Crouch");
const FName TAG_ABILITY_SHOOT("Ability.Shoot");
const FName TAG_ABILITY_PICKUP("Ability.PickUp");
const FName TAG_ABILITY_DROP("Ability.Drop");
const FName TAG_ABILITY_AIM("Ability.Aim");
const FName TAG_ABILITY_SHOOT_PROJECTILE("Ability.Shoot.Projectile");
const FName TAG_ABILITY_PICKUP_INDICATOR("Ability.PickupIndicator");
const FName TAG_ABILITY_RELOAD("Ability.Reload");
const FName TAG_ABILITY_INVENTORY("Ability.Inventory");

const FName TAG_STATE_INVULNERABLE = FName("State.Invulnerable");
const FName TAG_STATE_AIMING = FName("State.Aiming");
const FName TAG_STATE_DEAD = FName("State.Dead");
const FName TAG_STATE_SPRINTING = FName("State.Sprinting");
const FName TAG_STATE_RELOADING = FName("State.Reloading");

const FName TAG_GAMEPLAYCUE_JUMP("GameplayCue.Jump");
const FName TAG_GAMEPLAYCUE_SPRINT("GameplayCue.Sprint");
const FName TAG_GAMEPLAYCUE_SHOOT("GameplayCue.Shoot"); //executed from GE_GrenadeLauncherShoot
const FName TAG_GAMEPLAYCUE_DISSOLVE("GameplayCue.Dissolve");

const FName TAG_EVENT_MONTAGE_SHOOT("Event.Montage.Shoot");
const FName TAG_EVENT_MONTAGE_RELOAD_RELOAD("Event.Montage.Reload.Reload");
const FName TAG_EVENT_MONTAGE_RELOAD_AMMO_OUT("Event.Montage.Reload.AmmoOut");
const FName TAG_EVENT_MONTAGE_RELOAD_AMMO_IN("Event.Montage.Reload.AmmoIn");

const FName TAG_EVENT_SET_OBJECT_TO_PICKUP_INDICATOR("Event.PickupIndicator.Set");
const FName TAG_EVENT_INPUT_PICKUP("Event.Input.PickUp");
const FName TAG_EVENT_INPUT_JUMP("Event.Input.Jump");
const FName TAG_EVENT_INPUT_CROUCH("Event.Input.Crouch");
const FName TAG_EVENT_INPUT_STANDUP("Event.Input.StandUp");
const FName TAG_EVENT_INPUT_SPRINT("Event.Input.Sprint");
const FName TAG_EVENT_INPUT_WALK("Event.Input.Walk");
const FName TAG_EVENT_INPUT_DROP("Event.Input.Drop");
const FName TAG_EVENT_INPUT_START_FIRING("Event.Input.StartFiring");
const FName TAG_EVENT_INPUT_STOP_FIRING("Event.Input.StopFiring");
const FName TAG_EVENT_INPUT_SWAP_WEAPONS("Event.Input.SwapWeapons");
const FName TAG_EVENT_INPUT_SHOW_INVENTORY("Event.Input.ShowInventory");
const FName TAG_EVENT_INPUT_HIDE_INVENTORY("Event.Input.HideInventory");
const FName TAG_EVENT_INPUT_START_AIMING("Event.Input.StartAiming");
const FName TAG_EVENT_INPUT_STOP_AIMING("Event.Input.StopAiming");
const FName TAG_EVENT_INPUT_RELOAD("Event.Input.Reload");

const FName TAG_EFFECTS_WEAPON("Effects.Weapon.Cooldown");
const FName TAG_EFFECTS_DISSOLVE("Effects.Dissolve");

#include "BattleRoyale/core/Abilities/AbilityShootProjectileGun.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "AbilitiesInput.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagsList.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameplayEffects/CooldownGameplayEffect.h"

UAbilityShootProjectileGun::UAbilityShootProjectileGun()
{
	AbilityInputID = EAbilityInputID::Fire;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	//Esto estaba pensado para que fuera LocalOnly. Pero si es LocalOnly, el cooldown no funciona
	//cuando se trata de un cliente porque tiene que ser la autoridad.
	//Lo he puesto en LocalPredicted, y parece que funciona todo bien. Pero hay que revisar
	//que no se esté haciendo algo mal.
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_SHOOT_PROJECTILE));

	CooldownGameplayEffectClass = UCooldownGameplayEffect::StaticClass();
}

void UAbilityShootProjectileGun::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo,
                                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                                 const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	//if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		mCharacter = GetCharacter(ActorInfo);
		
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}

		if (mCharacter != nullptr)
		{
			SubscribeToEventMontageShoot(mCharacter);
			CreateTaskPlayMontageShooting(mCharacter, ActorInfo);
		}
	}
}

bool UAbilityShootProjectileGun::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo,
                                        const FGameplayTagContainer* SourceTags,
                                        const FGameplayTagContainer* TargetTags,
                                        OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
	/*const auto character = GetCharacter(ActorInfo);

	return character != nullptr && character->CanShoot();*/
}

const FGameplayTagContainer* UAbilityShootProjectileGun::GetCooldownTags() const
{
	const auto parentTags = Super::GetCooldownTags();

	const auto tagContainer = const_cast<FGameplayTagContainer*>(&CooldownTagContainer);
	tagContainer->Reset();
	if(parentTags)
	{
		tagContainer->AppendTags(*parentTags);
	}
	
	tagContainer->AppendTags(GetWeaponCooldownGameplayTags());
	
	return tagContainer;
}

void UAbilityShootProjectileGun::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	const auto cooldownGameplayEffect = Cast<UCooldownGameplayEffect>(GetCooldownGameplayEffect());
	if(cooldownGameplayEffect)
	{
		const auto specHandle = MakeOutgoingGameplayEffectSpec(cooldownGameplayEffect->GetClass(), GetAbilityLevel());
		specHandle.Data.Get()->DynamicGrantedTags.AppendTags(GetWeaponCooldownGameplayTags());
		cooldownGameplayEffect->SetDuration(GetWeaponCooldownDuration());
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, specHandle);
	}

	//Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo);
}

FGameplayTagContainer UAbilityShootProjectileGun::GetWeaponCooldownGameplayTags() const
{
	if(mCharacter)
	{
		const auto weapon = mCharacter->GetEquippedWeapon();
		check(weapon);
		return weapon->GetCooldownTags();
	}
	
	return FGameplayTagContainer();
}

float UAbilityShootProjectileGun::GetWeaponCooldownDuration() const
{
	if(mCharacter)
	{
		const auto weapon = mCharacter->GetEquippedWeapon();
		check(weapon);
		return weapon->GetCooldownTime();
	}
	return 0.0f;
}

void UAbilityShootProjectileGun::SubscribeToEventMontageShoot(const IICharacter* character)
{
	UAbilityTask_WaitGameplayEvent* newTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this, FGameplayTag::RequestGameplayTag(TAG_EVENT_MONTAGE_SHOOT), nullptr, true);
	newTask->EventReceived.AddDynamic(this, &UAbilityShootProjectileGun::OnEventMontageShootReceived);
	newTask->Activate();

	
	/* METHOD Another way to register to the event without using a task
	if(!mEventMontageShootHandle.IsValid())
	{
		const auto abilitySystemInterface = character->GetAbilitySystemComponent();
		mEventMontageShootHandle = abilitySystemInterface->GetAbilitySystemComponent()->AddGameplayEventTagContainerDelegate(
			FGameplayTagContainer(FGameplayTag::RequestGameplayTag(FName("Event.Montage.Shoot"))),
			FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UAbilityShootProjectileGun::OnEventMontageShootReceived));
	}*/
}

void UAbilityShootProjectileGun::CreateTaskPlayMontageShooting(const IICharacter* character, const FGameplayAbilityActorInfo* ActorInfo)
{
/*
	animInstance->Montage_Play(character->GetShootingMontage());
	
	animInstance->Montage_JumpToSection(sectionName);*/

	const auto sectionName = character->IsAiming() ? FName("AimingFire") : FName("Fire");
	const auto taskPlayMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		character->GetShootingMontage(),
		1.0,
		sectionName,
		true);
	
	taskPlayMontage->OnCancelled.AddDynamic(this, &UAbilityShootProjectileGun::OnMontageCancelled);
	taskPlayMontage->OnInterrupted.AddDynamic(this, &UAbilityShootProjectileGun::OnMontageCancelled);
	taskPlayMontage->OnCompleted.AddDynamic(this, &UAbilityShootProjectileGun::OnMontageCompleted);
	taskPlayMontage->OnBlendOut.AddDynamic(this, &UAbilityShootProjectileGun::OnMontageCompleted);
	taskPlayMontage->ReadyForActivation();
}

void UAbilityShootProjectileGun::OnMontageCompleted()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, FString("AbilityShootProjectileGun::OnMontageCompleted"));
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAbilityShootProjectileGun::OnMontageCancelled()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString("AbilityShootProjectileGun::OnMontageCancelled"));
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UAbilityShootProjectileGun::OnEventMontageShootReceived(const FGameplayEventData Payload)
{
	//Esto solo se ejecutará en el instigador del disparo porque es el que recibe el notify.
	//local y server si es un cliente
	//server si es el server
	/*
	const auto remoteRole = CurrentActorInfo->AvatarActor->GetRemoteRole();
	const auto localRole = CurrentActorInfo->AvatarActor->GetLocalRole();
	GEngine->AddOnScreenDebugMessage(
		-1,
		3,
		FColor::Cyan,
		FString::Printf(TEXT("AbilityShootProjectileGun::OnEventMontageShootReceived localRole %d, RemoteRole %d"),
			localRole,
			remoteRole ));*/
	if(mCharacter != nullptr)
	{
		mCharacter->Shoot();
	}
}
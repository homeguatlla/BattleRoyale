
#include "BattleRoyale/core/Abilities/AbilityShootProjectileGun.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "AbilitiesInput.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagsList.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameplayAbilitySystem/IAbilitySystemInterfaceBase.h"
#include "BattleRoyale/core/GameplayEffects/CooldownGameplayEffect.h"
#include "BattleRoyale/core/PickableObjects/Weapons/IWeapon.h"

UAbilityShootProjectileGun::UAbilityShootProjectileGun()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	//Esto estaba pensado para que fuera LocalOnly. Pero si es LocalOnly, el cooldown no funciona
	//cuando se trata de un cliente porque tiene que ser la autoridad.
	//Lo he puesto en LocalPredicted. Pero ahora el que hace el shoot es el locallycontrolled,
	//no queremos que el servidor dispare porque no puede calcular la dirección del disparo.
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_SHOOT_PROJECTILE));

	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_SPRINT));
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_SPRINT));

	CooldownGameplayEffectClass = UCooldownGameplayEffect::StaticClass();

	FAbilityTriggerData triggerDataToAdd;
	triggerDataToAdd.TriggerTag = FGameplayTag::RequestGameplayTag(TAG_EVENT_INPUT_START_FIRING);
	triggerDataToAdd.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(triggerDataToAdd);
}

void UAbilityShootProjectileGun::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo,
                                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                                 const FGameplayEventData* TriggerEventData)
{
	//No hay que llamar al super porque ya hacemos el commit aquí.
	//Sino estaríamos haciendo dos commits y dejaría de funcionar bien.
	//El commit es necesario para que se evalue el cooldown y el cost

	//if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		mCharacter = GetCharacter(ActorInfo);

		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		if (mCharacter != nullptr)
		{
			const auto abilitySystemComponent = mCharacter->GetAbilitySystemComponentBase();
			check(abilitySystemComponent);
			m_StopShootingHandle = abilitySystemComponent->RegisterGameplayEvent(
					FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TAG_EVENT_INPUT_STOP_FIRING)),
					FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnStopShooting));
			if (mCharacter->GetGunComponent()->IsAiming())
			{
				//if aiming we don't want to play shooting montage
				Shoot();
				EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			}
			else
			{
				SubscribeToEventMontageShoot(mCharacter);
				//Play shooting montage
				CreateTaskPlayMontageShooting(mCharacter, ActorInfo);
			}
		}
	}
}

bool UAbilityShootProjectileGun::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                    const FGameplayAbilityActorInfo* ActorInfo,
                                                    const FGameplayTagContainer* SourceTags,
                                                    const FGameplayTagContainer* TargetTags,
                                                    FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const auto character = GetCharacter(ActorInfo);
	if (!character)
	{
		return false;
	}
	const auto gunComponent = character->GetGunComponent();
	if(!gunComponent)
	{
		return false;
	}
	return gunComponent->HasWeaponEquipped();
}

const FGameplayTagContainer* UAbilityShootProjectileGun::GetCooldownTags() const
{
	const auto parentTags = Super::GetCooldownTags();

	const auto tagContainer = const_cast<FGameplayTagContainer*>(&CooldownTagContainer);
	tagContainer->Reset();
	if (parentTags)
	{
		tagContainer->AppendTags(*parentTags);
	}

	tagContainer->AppendTags(GetWeaponCooldownGameplayTags());

	return tagContainer;
}

void UAbilityShootProjectileGun::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo,
                                               const FGameplayAbilityActivationInfo ActivationInfo) const
{
	//No es necesario llamar al super, que lo que hace es hacer el applygameplayeffectspectoowner pero del
	//que pudiera tener definido en bp
	const auto cooldownGameplayEffect = Cast<UCooldownGameplayEffect>(GetCooldownGameplayEffect());
	if (cooldownGameplayEffect)
	{
		const auto specHandle = MakeOutgoingGameplayEffectSpec(cooldownGameplayEffect->GetClass(), GetAbilityLevel());
		specHandle.Data.Get()->DynamicGrantedTags.AppendTags(GetWeaponCooldownGameplayTags());
		cooldownGameplayEffect->SetDuration(GetWeaponCooldownDuration());
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, specHandle);
	}
}

FGameplayTagContainer UAbilityShootProjectileGun::GetWeaponCooldownGameplayTags() const
{
	if (mCharacter)
	{
		const auto gunComponent = mCharacter->GetGunComponent();
		if (gunComponent->HasWeaponEquipped())
		{
			const auto weapon = gunComponent->GetEquippedWeapon();
			check(weapon);
			return weapon->GetCooldownTags();
		}
	}

	return FGameplayTagContainer();
}

float UAbilityShootProjectileGun::GetWeaponCooldownDuration() const
{
	if (mCharacter)
	{
		const auto gunComponent = mCharacter->GetGunComponent();
		const auto weapon = gunComponent->GetEquippedWeapon();
		check(weapon);
		return weapon->GetCooldownTime();
	}
	return 0.0f;
}

void UAbilityShootProjectileGun::SubscribeToEventMontageShoot(const IICharacter* character)
{
	if (waitGameplayEventTask)
	{
		waitGameplayEventTask->EndTask();
	}

	waitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag(TAG_EVENT_MONTAGE_SHOOT),
		nullptr,
		true);
	waitGameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnEventMontageShootReceived);
	waitGameplayEventTask->Activate();

	/* METHOD Another way to register to the event without using a task
	if(!mEventMontageShootHandle.IsValid())
	{
		const auto abilitySystemInterface = character->GetAbilitySystemComponent();
		mEventMontageShootHandle = abilitySystemInterface->GetAbilitySystemComponent()->AddGameplayEventTagContainerDelegate(
			FGameplayTagContainer(FGameplayTag::RequestGameplayTag(FName("Event.Montage.Shoot"))),
			FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UAbilityShootProjectileGun::OnEventMontageShootReceived));
	}*/
}

void UAbilityShootProjectileGun::CreateTaskPlayMontageShooting(const IICharacter* character,
                                                               const FGameplayAbilityActorInfo* ActorInfo)
{
	const auto gunComponent = character->GetGunComponent();
	const auto sectionName = gunComponent->IsAiming() ? FName("AimingFire") : FName("Fire");
	const auto taskPlayMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		gunComponent->GetShootingMontage(),
		1.0,
		sectionName,
		true);

	taskPlayMontage->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);
	taskPlayMontage->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageCancelled);
	taskPlayMontage->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
	taskPlayMontage->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageCompleted);
	taskPlayMontage->ReadyForActivation();
}

void UAbilityShootProjectileGun::OnStopShooting(FGameplayTag gameplayTag, const FGameplayEventData* payload)
{
	const auto abilitySystemComponent = mCharacter->GetAbilitySystemComponentBase();
	check(abilitySystemComponent);
	abilitySystemComponent->UnRegisterGameplayEvent(
		FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TAG_EVENT_INPUT_STOP_FIRING)),
		m_StopShootingHandle);
	K2_CancelAbility();
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
	Shoot();
}

void UAbilityShootProjectileGun::Shoot() const
{
	if (!mCharacter || !IsLocallyControlled())
	{
		return;
	}

	mCharacter->Shoot();
}

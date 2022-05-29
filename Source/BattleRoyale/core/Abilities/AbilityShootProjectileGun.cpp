
#include "BattleRoyale/core/Abilities/AbilityShootProjectileGun.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "AbilitiesInput.h"
#include "AbilitySystemComponent.h"
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
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Shoot.Projectile")));

	CooldownGameplayEffectClass = UCooldownGameplayEffect::StaticClass();
}

void UAbilityShootProjectileGun::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo,
                                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                                 const FGameplayEventData* TriggerEventData)
{
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
	const auto character = GetCharacter(ActorInfo);

	return character != nullptr && character->CanShoot();
}

void UAbilityShootProjectileGun::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

// Epic's comment
/**
 *	Canceling an non instanced ability is tricky. Right now this works for Jump since there is nothing that can go wrong by calling
 *	StopJumping() if you aren't already jumping. If we had a montage playing non instanced ability, it would need to make sure the
 *	Montage that *it* played was still playing, and if so, to cancel it. If this is something we need to support, we may need some
 *	light weight data structure to represent 'non intanced abilities in action' with a way to cancel/end them.
 */
void UAbilityShootProjectileGun::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UAbilityShootProjectileGun::CancelAbility, Handle, ActorInfo,
		                                                      ActivationInfo, bReplicateCancelAbility));
		return;
	}
	
	/* METHOD Unregister the delegate if not using a task to listen the event
	const auto character = GetCharacter(ActorInfo);
	if (character != nullptr)
	{
		const auto abilitySystemInterface = character->GetAbilitySystemComponent();
		abilitySystemInterface->GetAbilitySystemComponent()->RemoveGameplayEventTagContainerDelegate(FGameplayTagContainer(FGameplayTag::RequestGameplayTag(FName("Event.Montage.Shoot"))), mEventMontageShootHandle);
		
		mEventMontageShootHandle.Reset();
	}*/

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
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

void UAbilityShootProjectileGun::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAbilityShootProjectileGun::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UAbilityShootProjectileGun::OnEventMontageShootReceived(const FGameplayEventData Payload)
{
	if(mCharacter != nullptr)
	{
		//mCharacter->ServerShoot();
		mCharacter->Shoot();
	}
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
			this, FGameplayTag::RequestGameplayTag(FName("Event.Montage.Shoot")), nullptr, true);
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
	ActorInfo->SkeletalMeshComponent->AnimScriptInstance = character->GetAnimationInstance();
	const auto animInstance = ActorInfo->GetAnimInstance();
	check(animInstance);

	//const auto abilitySystemInterface = character->GetAbilitySystemComponentBase();
	//abilitySystemInterface->SetSimulatedMontage(character->GetSimulatedShootingMontage());

	//Ejecutamos el play montage solo en local.
	animInstance->Montage_Play(character->GetShootingMontage(), 1.0);

	//Si hacemos un task, este se replica y entonces no funciona bien porque ejecuta un montage en primera persona
	//y al replicar tiene que ser uno en tercera persona.
	//Esto se puede solucionar sobreescribiendo los unos métodos del abilitycomponentsystem pero
	//genera un poco de código -raro- pues habría que crear un método que dado un montage en 1p te devuelva el
	//equivalente en 3p. Hay que ver si vale la pena. Por ahora no lo vamos a usar.
	//Tener en cuenta que el ability sistem component está en el playerState y habría que aumentar
	//el netfrecuencyUpdate
	/*const auto taskPlayMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		character->GetShootingMontage(),
		1.0,
		NAME_None,
		false);
	
	taskPlayMontage->OnCancelled.AddDynamic(this, &UAbilityShootProjectileGun::OnMontageCancelled);
	taskPlayMontage->OnInterrupted.AddDynamic(this, &UAbilityShootProjectileGun::OnMontageCancelled);
	taskPlayMontage->OnCompleted.AddDynamic(this, &UAbilityShootProjectileGun::OnMontageCompleted);
	taskPlayMontage->OnBlendOut.AddDynamic(this, &UAbilityShootProjectileGun::OnMontageCompleted);
	taskPlayMontage->ReadyForActivation();*/
}

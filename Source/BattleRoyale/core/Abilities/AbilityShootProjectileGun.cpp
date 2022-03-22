
#include "BattleRoyale/core/Abilities/AbilityShootProjectileGun.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "AbilitiesInput.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/GameplayAbilitySystem/AbilitySystemComponentBase.h"

UAbilityShootProjectileGun::UAbilityShootProjectileGun()
{
	AbilityInputID = EAbilityInputID::Fire;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Shoot.Projectile")));
}

void UAbilityShootProjectileGun::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo,
                                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                                 const FGameplayEventData* TriggerEventData)
{
	//if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}

		mCharacter = GetCharacter(ActorInfo);
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

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	const auto character = GetCharacter(ActorInfo);
	if (character != nullptr)
	{
		
	}
}

void UAbilityShootProjectileGun::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAbilityShootProjectileGun::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UAbilityShootProjectileGun::OnEventMontageShootReceived(FGameplayTag EventTag, const FGameplayEventData* Payload) const
{
	if(mCharacter != nullptr && mCharacter->IsAlive())
	{
		//mCharacter->ServerShoot();
		mCharacter->Shoot();
	}
}

void UAbilityShootProjectileGun::SubscribeToEventMontageShoot(const IICharacter* character)
{
	if(!mEventMontageShootHandle.IsValid())
	{
		const auto abilitySystemInterface = character->GetAbilitySystemComponent();
		mEventMontageShootHandle = abilitySystemInterface->GetAbilitySystemComponent()->AddGameplayEventTagContainerDelegate(
			FGameplayTagContainer(FGameplayTag::RequestGameplayTag(FName("Event.Montage.Shoot"))),
			FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UAbilityShootProjectileGun::OnEventMontageShootReceived));
	}
}

void UAbilityShootProjectileGun::CreateTaskPlayMontageShooting(const IICharacter* character, const FGameplayAbilityActorInfo* ActorInfo)
{
	ActorInfo->SkeletalMeshComponent->AnimScriptInstance = character->GetAnimationInstance();
	const auto animInstance = ActorInfo->GetAnimInstance();
	check(animInstance);

	//const auto abilitySystemInterface = character->GetAbilitySystemComponentBase();
	//abilitySystemInterface->SetSimulatedMontage(character->GetSimulatedShootingMontage());

	//Ejecutamos el play montage solo en cliente.
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

// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystemComponentBase.h"

#include "AbilitySystemInterface.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/Character/ICharacter.h"

void UAbilitySystemComponentBase::SetSimulatedMontage(UAnimMontage* montage)
{
	FGameplayAbilityRepAnimMontage animMontage;
	animMontage.AnimMontage = montage;
	
	SetRepAnimMontageInfo(animMontage);
}

FActiveGameplayEffectHandle UAbilitySystemComponentBase::ApplyGameplayEffectToTarget(const TSubclassOf<UGameplayEffect>& effect, const IICharacter* target)
{
	if(const auto targetAbilityComponent = target->GetAbilitySystemComponent())
	{
		auto effectContext = MakeEffectContext();
		auto owner = GetOwnerActor();
		effectContext.AddSourceObject(GetOwnerActor());

		const auto gameplayEffectHandle = MakeOutgoingSpec(effect, 1, effectContext);
		if(gameplayEffectHandle.IsValid())
		{
			return ApplyGameplayEffectSpecToTarget(*gameplayEffectHandle.Data.Get(), targetAbilityComponent->GetAbilitySystemComponent());
		}
	}
	return FActiveGameplayEffectHandle();
}

FActiveGameplayEffectHandle UAbilitySystemComponentBase::ApplyGameplayEffectToSelf(const TSubclassOf<UGameplayEffect>& effectClass)
{
	auto effectContext = MakeEffectContext();
	auto owner = GetOwnerActor();
	effectContext.AddSourceObject(GetOwnerActor());

	return UAbilitySystemComponent::ApplyGameplayEffectToSelf(effectClass->GetDefaultObject<UGameplayEffect>(), 1, effectContext);
}

/*
static int kk = 0;
float UAbilitySystemComponentBase::PlayMontage(UGameplayAbility* AnimatingAbility,
	FGameplayAbilityActivationInfo ActivationInfo, UAnimMontage* Montage, float InPlayRate, FName StartSectionName,
	float StartTimeSeconds)
{
	float Duration = -1.f;

	UAnimInstance* AnimInstance = AbilityActorInfo.IsValid() ? AbilityActorInfo->GetAnimInstance() : nullptr;
	if (AnimInstance && Montage)
	{
		Duration = AnimInstance->Montage_Play(Montage, InPlayRate, EMontagePlayReturnType::MontageLength, StartTimeSeconds);
		if (Duration > 0.f)
		{
			if (LocalAnimMontageInfo.AnimatingAbility && LocalAnimMontageInfo.AnimatingAbility != AnimatingAbility)
			{
				// The ability that was previously animating will have already gotten the 'interrupted' callback.
				// It may be a good idea to make this a global policy and 'cancel' the ability.
				// 
				// For now, we expect it to end itself when this happens.
			}

			if (Montage->HasRootMotion() && AnimInstance->GetOwningActor())
			{
				UE_LOG(LogRootMotion, Log, TEXT("UAbilitySystemComponent::PlayMontage %s, Role: %s")
					, *GetNameSafe(Montage)
					, *UEnum::GetValueAsString(TEXT("Engine.ENetRole"), AnimInstance->GetOwningActor()->GetLocalRole())
					);
			}

			LocalAnimMontageInfo.AnimMontage = Montage;
			LocalAnimMontageInfo.AnimatingAbility = AnimatingAbility;
			LocalAnimMontageInfo.PlayBit = !LocalAnimMontageInfo.PlayBit;
			
			if (AnimatingAbility)
			{
				AnimatingAbility->SetCurrentMontage(Montage);
			}
			
			// Start at a given Section.
			if (StartSectionName != NAME_None)
			{
				AnimInstance->Montage_JumpToSection(StartSectionName, Montage);
			}

			// Replicate to non owners
			if (IsOwnerActorAuthoritative())
			{
				
				FGameplayAbilityRepAnimMontage& MutableRepAnimMontageInfo = GetRepAnimMontageInfo_Mutable();

				// Those are static parameters, they are only set when the montage is played. They are not changed after that.
				const auto character = Cast<IICharacter>(AvatarActor);
				MutableRepAnimMontageInfo.AnimMontage = character->GetSimulatedShootingMontage();
				
				MutableRepAnimMontageInfo.SectionIdToPlay = 0;
				if (MutableRepAnimMontageInfo.AnimMontage && StartSectionName != NAME_None)
				{
					// we add one so INDEX_NONE can be used in the on rep
					MutableRepAnimMontageInfo.SectionIdToPlay = MutableRepAnimMontageInfo.AnimMontage->GetSectionIndex(StartSectionName) + 1;
				}

				// Update parameters that change during Montage life time.
				AnimMontage_UpdateReplicatedData();
				
				MutableRepAnimMontageInfo.AnimMontage = character->GetSimulatedShootingMontage();
				MutableRepAnimMontageInfo.ForcePlayBit = kk++;//!MutableRepAnimMontageInfo.ForcePlayBit;
				//MutableRepAnimMontageInfo.NextSectionID ++;
				UE_LOG(LogCharacter, Log, TEXT("PlayMontage force bit %d, nextSection ID %d "), MutableRepAnimMontageInfo.ForcePlayBit, MutableRepAnimMontageInfo.NextSectionID);
				// Force net update on our avatar actor
				if (AbilityActorInfo->AvatarActor != nullptr)
				{
					AbilityActorInfo->AvatarActor->ForceNetUpdate();
				}
			}
			else
			{
				// If this prediction key is rejected, we need to end the preview
				FPredictionKey PredictionKey = GetPredictionKeyForNewAction();
				if (PredictionKey.IsValidKey())
				{
					PredictionKey.NewRejectedDelegate().BindUObject(this, &UAbilitySystemComponent::OnPredictiveMontageRejected, Montage);
				}
			}
		}
	}

	return Duration;
}

float UAbilitySystemComponentBase::PlayMontageSimulated(UAnimMontage* Montage, float InPlayRate, FName StartSectionName)
{
	const auto character = Cast<IICharacter>(GetAvatarActor());
	
	if(GetAvatarActor()->GetLocalRole() != ROLE_SimulatedProxy)
	{
		return 0.0f;
	}
	
	Montage = character->GetSimulatedShootingMontage();
	float Duration = -1.f;
	UAnimInstance* AnimInstance = AbilityActorInfo.IsValid() ? AbilityActorInfo->GetAnimInstance() : nullptr;
	if (AnimInstance && Montage)
	{
		UE_LOG(LogCharacter, Log, TEXT("Montage name = %s, animInstanceName = %s"), *Montage->GetName(), *AnimInstance->GetName());
		Duration = AnimInstance->Montage_Play(Montage, InPlayRate);
		if (Duration > 0.f)
		{
			//LocalAnimMontageInfo.AnimMontage = Montage;
		}
	}

	return Duration;
}*/

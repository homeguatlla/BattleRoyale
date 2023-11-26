#pragma once
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/GameplayAbilitySystem/IAbilitySystemInterfaceBase.h"
#include "BattleRoyale/core/PickableObjects/Weapons/ProjectileBase.h"
#include "Kismet/GameplayStatics.h"

namespace utils
{
class BATTLEROYALE_API UtilsLibrary
{
	public:
		template <class InterfaceTypeSource, class InterfaceTypeTarget = InterfaceTypeSource>
		static InterfaceTypeTarget* CreateHUDFromClass(int hudIndex, const FString& name, APlayerController* playerController, TSubclassOf<InterfaceTypeSource> hudClass, bool autoAddToViewport = true, int32 zOrder = 0)
		{
			InterfaceTypeTarget* widget = InstantiateHUD<InterfaceTypeSource, InterfaceTypeTarget>(
			hudIndex, 
			name, 
			playerController,
			hudClass, 
			autoAddToViewport, 
			zOrder);
			
			return widget;
		}

		template <class InterfaceTypeSource, class InterfaceTypeTarget>
		static InterfaceTypeTarget* InstantiateHUD(int hudIndex, const FString& name, APlayerController* playerController, TSubclassOf<UUserWidget> hudClass, bool autoAddToViewport, int32 zOrder)
		{
			//CreateWidget need different FName for each create otherwise it returns a pointer to the previously create with the same name.
				//refer to the link below
				//https://answers.unrealengine.com/questions/810228/strange-behaviour-of-uuserwidgetcreatewidgetinstan.html?sort=oldest

			const FString hudName = hudClass->GetName() + "_" + FString::FromInt(hudIndex) + "_" + name;
			UUserWidget* widget = CreateWidget<UUserWidget, APlayerController*>(playerController, hudClass, FName(*hudName));

			if (autoAddToViewport)
				widget->AddToViewport(zOrder == 0 ? hudIndex + 1 : zOrder);
			//widget->InitializeWidget();

			return Cast<InterfaceTypeTarget>(widget);
		}

		template<class T>
		static TArray<AActor*> GetAllActorsOfClass(UWorld* world, TSubclassOf<T> classToFind)
		{
			TArray<AActor*> foundActors;
			UGameplayStatics::GetAllActorsOfClass(world, classToFind, foundActors);
			
			return foundActors;
		}

		static TArray<FHitResult> TraceVisionSphere(
			UWorld* world,
	        const FVector& startLocation,
	        const FVector& endLocation,
	        const float radius,
	        const FQuat& rotation,
	        ECollisionChannel channel,
	        const FCollisionQueryParams& params)
		{
			check(world);
			
			const auto shape = FCollisionShape::MakeSphere(radius);
			return TraceSweepMultyByChannel(world, startLocation, endLocation, rotation, channel, shape, params);
		}

		static TArray<FHitResult> TraceVisionCapsule(
				UWorld* world,
				const FVector& startLocation,
				const FVector& endLocation,
				const float radius,
				const float height,
				const FQuat& rotation,
				ECollisionChannel channel,
				const FCollisionQueryParams& params)
		{
			check(world);
			
			const auto shape = FCollisionShape::MakeCapsule(radius, height);
			return TraceSweepMultyByChannel(world, startLocation, endLocation, rotation, channel, shape, params);
		}
	
		static TArray<FHitResult> TraceSweepMultyByChannel(
			UWorld* world,
			const FVector& startLocation,
			const FVector& endLocation,
			const FQuat& rotation,
			ECollisionChannel channel,
			const FCollisionShape& shape,
			const FCollisionQueryParams& params) 
		{
			check(world);
			
			TArray<FHitResult> outHits;

			//auto result = world->SweepMultiByObjectType(outHits, startLocation, endLocation, rotation, CHANNELOUT_LEFTSURROUND, shape, params);
			auto result = world->SweepMultiByChannel(
				outHits,
				startLocation,
				endLocation,
				rotation,
				channel,
				shape,
				params);

			return outHits;
		}
	
	static FHitResult TraceLineSingleByChannelToCrosshair(UWorld* world, APlayerController* playerController, float maxTraceDistance)
		{
			check(world);
			check(world->GetGameViewport());
			check(playerController);
	
			FVector2D viewportSize;
			world->GetGameViewport()->GetViewportSize(viewportSize);

			const auto crosshairLocation = viewportSize * 0.5f;

			FVector crossHairsWorldLocation, crossHairsWorldDirection;
			const auto isDeprojected = UGameplayStatics::DeprojectScreenToWorld(
				playerController,
				crosshairLocation,
				crossHairsWorldLocation,
				crossHairsWorldDirection);

			if(!isDeprojected)
			{
				UE_LOG(LogTemp, Warning, TEXT("[%s] TraceLineSingleByChannelToCrossHairs couldn't deproject Screen To World coordinates"), __FUNCTION__);
				return FHitResult();
			}
			
			const auto rayStart = crossHairsWorldLocation;
			//crossHairsWorldDirectionn is already normalized
			const auto rayEnd = rayStart + crossHairsWorldDirection * maxTraceDistance;
			FCollisionQueryParams params;
			params.AddIgnoredActor(playerController->GetPawn());
			
			FHitResult hitResult;
			world->LineTraceSingleByChannel(
				hitResult,
				rayStart,
				rayEnd,
				ECollisionChannel::ECC_Visibility,
				params);
			//DrawDebugLine(world, rayStart, rayEnd, FColor::Blue);
			
			return hitResult;
		}

	static FQuat LookAt(const FVector& sourcePoint, const FVector& destPoint)
		{
			FVector forwardVector = destPoint - sourcePoint;
			forwardVector.Normalize();

			const float dot = FVector::DotProduct(FVector::ForwardVector, forwardVector);

			//two opposite vectors, angle = 180 we proceed with UpVector and angle 180
			if (FMath::IsNearlyZero(FGenericPlatformMath::Abs(dot - (-1.0f))))
			{
				return FQuat(FVector::UpVector.X, FVector::UpVector.Y, FVector::UpVector.Z, PI);
			}
			//two coincident vectors, angle = 0, we proceed with identity
			else if (FMath::IsNearlyZero(FGenericPlatformMath::Abs(dot - (1.0f))))
			{
				return FQuat::Identity;
			}
			else
			{
				const float rotAngle = (float)FGenericPlatformMath::Acos(dot);
				FVector rotAxis = FVector::CrossProduct(FVector::ForwardVector, forwardVector);
				rotAxis.Normalize();
				return CreateQuatFromAxisAngle(rotAxis, rotAngle);
			}
		}

	static FQuat CreateQuatFromAxisAngle(const FVector& axis, float angle)
		{
			const float halfAngle = angle * .5f;
			const float s = static_cast<float>(FGenericPlatformMath::Sin(halfAngle));
			FQuat q;
			q.X = axis.X * s;
			q.Y = axis.Y * s;
			q.Z = axis.Z * s;
			q.W = static_cast<float>(FGenericPlatformMath::Cos(halfAngle));
			return q;
		}

	static FName GetValidMapName(const UWorld* world, const FName& mapName)
		{
			if(mapName.IsNone())
			{
				FString LevelName = world->GetMapName();
				LevelName.RemoveFromStart(world->StreamingLevelsPrefix);
		
				return *LevelName;
			}
	
			return mapName;
		}

	template<typename TargetDataClass, typename = typename TEnableIf<TIsDerivedFrom<TargetDataClass, FGameplayAbilityTargetData>::IsDerived>::Type>
	static bool RetrieveGameplayEventTargetData(const FGameplayEventData* Payload, TargetDataClass& data)
	{
		if(Payload->TargetData.Num() <= 0)
		{
			UE_LOG(LogTemp, Display, TEXT("[%s] RetrieveGameplayEventTargetData targetdata num <= 0"), __FUNCTION__);
			return false;
		}
	
		const auto targetData = Payload->TargetData.Data[0].Get();

		if(targetData == nullptr)
		{
			UE_LOG(LogTemp, Display, TEXT("[%s] RetrieveGameplayEventTargetData targetdata null"), __FUNCTION__);
			return false;
		}

		const auto currentTargetStruct = targetData->GetScriptStruct();
		const auto desiredTargetStruct = TargetDataClass::StaticStruct();

		if( currentTargetStruct->GetStructCPPName() != desiredTargetStruct->GetStructCPPName() )
		{
			UE_LOG(LogTemp, Display, TEXT("[%s] RetrieveGameplayEventTargetData targetdata different type"), __FUNCTION__);
			return false;
		}
	
		currentTargetStruct->InitializeStruct(&data);
		currentTargetStruct->CopyScriptStruct(&data, targetData);

		return true;
	}

	template<typename TargetDataClass, typename = typename TEnableIf<TIsDerivedFrom<TargetDataClass, FGameplayAbilityTargetData>::IsDerived>::Type>
	static void SendGameplayEventWithTargetData(const IICharacter* target, FGameplayTag eventTag, const AActor* instigator = nullptr, TargetDataClass* targetData = nullptr, float eventMagnitude = 0.0f)
	{
		if(!target)
		{
			return;
		}
		if(!target->GetAbilitySystemComponentBase())
		{
			return;
		}
			
		FGameplayEventData data;
		data.EventTag = eventTag;
		data.Instigator = instigator;
		data.EventMagnitude = eventMagnitude;
		if(targetData)
		{
			data.TargetData.Add(targetData);
		}

		target->GetAbilitySystemComponentBase()->SendGameplayEvent(eventTag, data);
	}
	
	static AProjectileBase* LaunchProjectile(const UObject* worldContextObject, TSubclassOf<class AProjectileBase> projectileClass, const FTransform& transform, AActor* owner, APawn* instigator)
	{
		const auto world = worldContextObject ? worldContextObject->GetWorld() : nullptr;
		if(!world || !instigator->HasAuthority())
		{
			return nullptr;
		}
		if(const auto projectile = world->SpawnActorDeferred<AProjectileBase>(projectileClass, transform, owner, instigator, ESpawnActorCollisionHandlingMethod::AlwaysSpawn))
		{
			projectile->FinishSpawning(transform);
			return projectile;
		}
		else
		{
			UE_LOG(LogWeapon, Error, TEXT("[%s][LaunchProjectile] Couldn't spawn the projectile"), *instigator->GetName());
		}
		return nullptr;
	}

	static void ApplyRadialDamage(const UObject* worldContextObject, AActor* damageCauser, const FVector& location, float radius,
		const TArray<TSubclassOf<class UGameplayEffect>>& damageEffects, const TArray<TEnumAsByte<EObjectTypeQuery>>& objectTypes, ETraceTypeQuery traceChannel)
	{
		TArray<AActor*> foundActors;
		const TArray<AActor*> actorsToIgnore = {damageCauser};

		//Find all actors arround	
		UKismetSystemLibrary::SphereOverlapActors(worldContextObject, location, radius, objectTypes, nullptr, actorsToIgnore, foundActors);
		for(const auto actor : foundActors)
		{
			FHitResult hitResult;

			//Trace from location to actor to know if there is no blocker between
			if(UKismetSystemLibrary::LineTraceSingle(worldContextObject, location, actor->GetActorLocation(), traceChannel, true, actorsToIgnore, EDrawDebugTrace::None, hitResult, true))
			{
				const auto target = hitResult.GetActor();
				if(target == actor)
				{
					const auto character = Cast<ACharacterBase>(target);
					if(!character)
					{
						return;
					}
					
					const auto abilitySystemComponent = character->GetAbilitySystemComponent()->GetAbilitySystemComponent();
					if(!abilitySystemComponent)
					{
						return;
					}
					FGameplayEffectContextHandle effectContext = abilitySystemComponent->MakeEffectContext();
					effectContext.AddInstigator(damageCauser, damageCauser);
					effectContext.AddSourceObject(damageCauser);
					for(auto effect : damageEffects)
					{
						const auto specHandle = abilitySystemComponent->MakeOutgoingSpec(effect, 1, effectContext);
						if(specHandle.IsValid())
						{
							const auto activeGEHandle = abilitySystemComponent->ApplyGameplayEffectSpecToSelf(*specHandle.Data.Get());
							if(!activeGEHandle.WasSuccessfullyApplied())
							{
								UE_LOG(LogWeapon, Error, TEXT("[ApplyRadialDamage] Couldn't apply GE to target %s"), *target->GetName());
							}
						}
					}
				}
			}
		}
	}
};
}

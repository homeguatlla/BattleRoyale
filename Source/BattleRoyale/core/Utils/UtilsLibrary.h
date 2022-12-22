#pragma once
#include "Abilities/GameplayAbilityTypes.h"
#include "BattleRoyale/core/Character/ICharacter.h"

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
			const auto shape = FCollisionShape::MakeSphere(radius);
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
	
		FGameplayAbilityTargetData* targetData = Payload->TargetData.Data[0].Get();

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
	static void SendGameplayEventWithTargetData(const IICharacter* target, FGameplayTag eventTag, TargetDataClass* targetData, const AActor* instigator = nullptr, float eventMagnitude = 0.0f)
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
		data.TargetData.Add(targetData);

		target->GetAbilitySystemComponentBase()->SendGameplayEvent(eventTag, data);
	}
};
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupSelectorComponent.h"

#include "BlueprintGameplayTagLibrary.h"
#include "VectorTypes.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/Abilities/GameplayTagsList.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/PickableObjects/IPickupObject.h"
#include "BattleRoyale/core/Utils/TargetDatas/TargetDataPickupObject.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UPickupSelectorComponent::UPickupSelectorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPickupSelectorComponent::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickEnabled(false);
}

void UPickupSelectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SelectPickupObject();
}

void UPickupSelectorComponent::InitializeServer()
{
	if(!GetOwner()->HasAuthority())
	{
		return;
	}

	RegisterOverlapEvents();
	
	const auto character = Cast<IICharacter>(GetOwner());
	if(!character)
	{
		return;
	}
	
	const auto abilitySystemComponentInterface = character->GetAbilitySystemComponentBase();
	if(!abilitySystemComponentInterface)
	{
		return;
	}
	
	if(InitializePickupIndicatorEffect)
	{
		if(!abilitySystemComponentInterface->ApplyGameplayEffectToSelf(InitializePickupIndicatorEffect).WasSuccessfullyApplied())
		{
			UE_LOG(LogCharacter, Warning, TEXT("[%s][UPickupSelectorComponent::InitializeComponent] pickup indicator initializing effect was not successfully applied"), *GetName());
		}
	}
}

void UPickupSelectorComponent::RegisterOverlapEvents()
{
	const auto character = Cast<ACharacterBase>(GetOwner());
	check(character);

	character->GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnCapsuleBeginOverlapServer);
	character->GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnCapsuleEndOverlapServer);
}

void UPickupSelectorComponent::SelectPickupObject()
{
	if(mPickableObjectsCollisionList.Num() == 1)
	{
		if(mSelectedPickableObject != mPickableObjectsCollisionList[0])
		{
			mSelectedPickableObject = mPickableObjectsCollisionList[0];
			NotifyOrCancelSelectedPickupObject();
		}
	}
	else if(mPickableObjectsCollisionList.Num() > 1)
	{
		//Order list by distance to character
		const auto characterLocation = GetOwner()->GetActorLocation();
		mPickableObjectsCollisionList.Sort([&characterLocation](const TScriptInterface<IPickupObject>& object1, const TScriptInterface<IPickupObject>& object2)
		{
			return FVector::Distance(object1->GetLocation(), characterLocation) < FVector::Distance(object2->GetLocation(), characterLocation);
		});
		//Order list by dot product to the camera direction
		const auto cameraDirection = GetOwner()->GetActorForwardVector();
		mPickableObjectsCollisionList.Sort([&characterLocation, &cameraDirection](const TScriptInterface<IPickupObject>& object1, const TScriptInterface<IPickupObject>& object2)
		{
			auto vectorObject1 = object1->GetLocation()-characterLocation;
			auto vectorObject2 = object2->GetLocation()-characterLocation;
			vectorObject1.Normalize();
			vectorObject2.Normalize();
			const auto dot1 = cameraDirection.Dot(vectorObject1);
			const auto dot2 = cameraDirection.Dot(vectorObject2);
			return dot1 > dot2;
		});
		if(mSelectedPickableObject != mPickableObjectsCollisionList[0])
		{
			mSelectedPickableObject = mPickableObjectsCollisionList[0];
			NotifyOrCancelSelectedPickupObject();
		}
	}
}

void UPickupSelectorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPickupSelectorComponent, mSelectedPickableObject);
}

void UPickupSelectorComponent::OnCapsuleBeginOverlapServer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->Implements<UPickupObject>())
	{
		mPickableObjectsCollisionList.Add(OtherActor);
		SetComponentTickEnabled(true);
	}
}

void UPickupSelectorComponent::OnCapsuleEndOverlapServer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
												 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor->Implements<UPickupObject>())
	{
		mPickableObjectsCollisionList.Remove(OtherActor);
		if(mPickableObjectsCollisionList.IsEmpty())
		{
			SetComponentTickEnabled(false);
			mSelectedPickableObject = nullptr;
			NotifyOrCancelSelectedPickupObject();
		}
	}
}

void UPickupSelectorComponent::OnRep_SelectedPickableObject()
{
	NotifyOrCancelSelectedPickupObject();
}

void UPickupSelectorComponent::NotifyOrCancelSelectedPickupObject() const
{
	CancelPickupIndicator(GetOwner());
	
	if(const auto character = Cast<IICharacter>(GetOwner()))
	{
		const auto abilitySystemComponent = character->GetAbilitySystemComponentBase();
		if(!abilitySystemComponent)
		{
			return;
		}
		if(mSelectedPickableObject)
		{
			utils::UtilsLibrary::SendGameplayEventWithTargetData<FTargetDataPickupObject>(
				character,
				FGameplayTag::RequestGameplayTag(TAG_EVENT_SET_OBJECT_TO_PICKUP_INDICATOR),
				GetOwner(),
				new FTargetDataPickupObject(mSelectedPickableObject->GetLocation(), Cast<APickableObjectBase>(mSelectedPickableObject.GetObject())));
		}
	}
}

void UPickupSelectorComponent::CancelPickupIndicator(const TScriptInterface<IICharacter> character) const
{
	check(character);
	
	const auto abilitySystemComponent = character->GetAbilitySystemComponentBase();
	
	//Remove the ability with tag pickup indicator
	FGameplayTagContainer cancelTags;
	UBlueprintGameplayTagLibrary::AddGameplayTag(cancelTags, FGameplayTag::RequestGameplayTag(TAG_ABILITY_PICKUP_INDICATOR));
	abilitySystemComponent->CancelAbilitiesWithTags(cancelTags);

	//En caso que enviemos effecto hay que quitarlo
	//playerState->GetAbilitySystemComponentInterface()->RemoveGameplayEffect(mPickupIndicatorEffectHandle);
}

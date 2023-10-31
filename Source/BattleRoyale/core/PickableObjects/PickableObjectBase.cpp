// Fill out your copyright notice in the Description page of Project Settings.


#include "PickableObjectBase.h"

#include "BlueprintGameplayTagLibrary.h"
#include "BattleRoyale/core/Abilities/GameplayTagsList.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameMode/PlayerState/PlayerStateBase.h"
#include "BattleRoyale/core/Utils/UtilsLibrary.h"
#include "BattleRoyale/core/Utils/TargetDatas/TargetDataPickupObject.h"
#include "BattleRoyale/BattleRoyale.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APickableObjectBase::APickableObjectBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bReplicates = true;
	
	// Create a gun mesh component
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PickableObject"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	Mesh->bCastDynamicShadow = false;
	Mesh->CastShadow = false;

	SetRootComponent(Mesh);
	
	Mesh->SetCollisionResponseToChannels(ECollisionResponse::ECR_Block);
	//Ignoring the collision with pawn, so characters can walk over.
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	//Not collision from construction
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	//Area sphere is used to detect overlaps with characters to show something and be able to pick up
	//Disabled for now. We only want to be enabled on server
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);

	DisableDetectionArea();

	//The pickupable object starts with the Value of its StaticData
	if(InventoryItemStaticData)
	{
		const auto inventoryItemStaticData = Cast<UInventoryItemStaticData>(InventoryItemStaticData->GetDefaultObject());
		Value = inventoryItemStaticData->GetValue();
	}
}

void APickableObjectBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APickableObjectBase, State);
	DOREPLIFETIME(APickableObjectBase, Value);
}

// Called when the game starts or when spawned
void APickableObjectBase::BeginPlay()
{
	Super::BeginPlay();
	
	if(HasAuthority())
	{
		EnableDetectionArea();
	}
}


void APickableObjectBase::ChangeState(EPickupObjectState state)
{
	switch (state)
	{
	case EPickupObjectState::Equipped:
		if(HasAuthority())
		{
			DisableDetectionArea();
		}
		SetEnableMeshPhysicsAndCollision(false);
		break;
	case EPickupObjectState::Dropped:
		if(HasAuthority())
		{
			EnableDetectionArea();
		}
		SetEnableMeshPhysicsAndCollision(true);
		break;
	case EPickupObjectState::Initial:
		break;
	default:
		break;
	}
	State = state;
}

bool APickableObjectBase::AttachToComponent(USkeletalMeshComponent* meshComponent,
                                          const FAttachmentTransformRules& attachmentRules, const FName& socketName)
{
	if(GetMesh() == nullptr)
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][APickupObjectBase::AttachToComponent] pickup object has no mesh"), *GetName());
		return false;
	}
	
	return GetMesh()->AttachToComponent(meshComponent, attachmentRules, socketName);
}

void APickableObjectBase::DetachFromComponent(const FDetachmentTransformRules& rules)
{
	if(GetMesh() == nullptr)
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][APickupObjectBase::DeattachToComponent] pickup object has no mesh"), *GetName());
		return;
	}
	
	GetMesh()->DetachFromComponent(rules);
}

void APickableObjectBase::SetValue(int value)
{
	if(HasAuthority())
	{
		Value = value;
	}
}

void APickableObjectBase::SetCharacterOwner(ACharacterBase* character)
{
	SetOwner(character);
}

void APickableObjectBase::OnEquipped()
{
	ChangeState(EPickupObjectState::Equipped);
	DoEquipped();
}

void APickableObjectBase::OnUnEquipped()
{
	ChangeState(EPickupObjectState::InInventory);
}

void APickableObjectBase::OnDropped()
{
	ChangeState(EPickupObjectState::Dropped);
}

void APickableObjectBase::EnableDetectionArea() const
{
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void APickableObjectBase::DisableDetectionArea() const
{
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AreaSphere->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
}

void APickableObjectBase::SetEnableMeshPhysicsAndCollision(bool enable) const
{
	GetMesh()->SetSimulatePhysics(enable);
	GetMesh()->SetEnableGravity(enable);
	GetMesh()->SetCollisionEnabled(enable ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
}

void APickableObjectBase::OnRep_State()
{
	ChangeState(State);

	//This code is because we can not attach the pickup object if it has physics enabled.
	//So, in client side once the state has been changed we can do the attach.
	//I don't know a better way to do it.
	if(State == EPickupObjectState::Equipped)
	{
		const auto character = Cast<ACharacterBase>(GetOwner());
		const auto isAttached = AttachToComponent(
					character->GetMesh(),
					FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
					character->GetRightHandSocketName());
		if(!isAttached)
		{
			UE_LOG(LogCharacter, Error, TEXT("[%s][ACharacterBase::Equip] pickup object not attached to the character"), *GetName());
		}	
	}
}
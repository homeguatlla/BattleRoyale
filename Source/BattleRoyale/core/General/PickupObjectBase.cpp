// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupObjectBase.h"

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
APickupObjectBase::APickupObjectBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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
	//AreaSphere->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
	//AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void APickupObjectBase::BeginPlay()
{
	Super::BeginPlay();
	
	if(HasAuthority())
	{
		EnableDetectionArea();
	}
}

void APickupObjectBase::ChangeState(EPickupObjectState state)
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

bool APickupObjectBase::AttachToComponent(USkeletalMeshComponent* meshComponent,
                                          const FAttachmentTransformRules& attachmentRules, const FName& socketName)
{
	if(GetMesh() == nullptr)
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][APickupObjectBase::AttachToComponent] pickup object has no mesh"), *GetName());
		return false;
	}
	
	return GetMesh()->AttachToComponent(meshComponent, attachmentRules, socketName);
}

void APickupObjectBase::DetachFromComponent(const FDetachmentTransformRules& rules)
{
	if(GetMesh() == nullptr)
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][APickupObjectBase::DeattachToComponent] pickup object has no mesh"), *GetName());
		return;
	}
	
	GetMesh()->DetachFromComponent(rules);
}
/*
FVector APickupObjectBase::GetPickupWidgetLocation() const
{
	FVector boundingBoxOrigin, boundingBoxExtend;
	//TODO Nos devuelve las bounds de la esfera de colision, y no nos interesa porque el texto saldrá a un radio
	//demasiado alejado del pick up object.
	//Queremos los bounds de la mesh. Pero no hay manera. Igual hay que pillar la local bound de la mesh y aplicarle la rotacion
	//del actor. Pensarlo bien, buscar ejemplos.
	
	//GetActorBounds(false, boundingBoxOrigin, boundingBoxExtend);
	boundingBoxOrigin = GetActorLocation();
	boundingBoxExtend = GetMesh()->GetLocalBounds().BoxExtent;
	const auto height = boundingBoxExtend.Z;
	const auto objectLocation = GetActorLocation();
	DrawDebugBox(GetWorld(), boundingBoxOrigin , boundingBoxExtend,FColor::Green, false, 20);
	DrawDebugSphere(GetWorld(), boundingBoxOrigin + FVector(0.0f, 0.0f, height), 3, 30, FColor::Red, false, 20);
	return boundingBoxOrigin;// + FVector(0.0f, 0.0f, height);
}*/

void APickupObjectBase::SetValue(int value)
{
	if(HasAuthority())
	{
		Value = value;
	}
}

void APickupObjectBase::SetCharacterOwner(ACharacterBase* character)
{
	SetOwner(character);
}

void APickupObjectBase::OnEquipped()
{
	ChangeState(EPickupObjectState::Equipped);
	DoEquipped();
}

void APickupObjectBase::OnUnEquipped()
{
	ChangeState(EPickupObjectState::InInventory);
}

void APickupObjectBase::OnDropped()
{
	ChangeState(EPickupObjectState::Dropped);
}

void APickupObjectBase::EnableDetectionArea() const
{
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlapServer);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnSphereEndOverlapServer);
}

void APickupObjectBase::DisableDetectionArea() const
{
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AreaSphere->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
	
	AreaSphere->OnComponentBeginOverlap.RemoveAll(this);
	AreaSphere->OnComponentEndOverlap.RemoveAll(this);
}

void APickupObjectBase::SetEnableMeshPhysicsAndCollision(bool enable) const
{
	GetMesh()->SetSimulatePhysics(enable);
	GetMesh()->SetEnableGravity(enable);
	GetMesh()->SetCollisionEnabled(enable ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
}

void APickupObjectBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APickupObjectBase, State);
	DOREPLIFETIME(APickupObjectBase, Value);
}

void APickupObjectBase::OnRep_State()
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

void APickupObjectBase::OnSphereOverlapServer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(IsEquipped())
	{
		return;
	}
	
	if(const auto character = Cast<ACharacterBase>(OtherActor))
	{
		const auto playerState = Cast<APlayerStateBase>(character->GetPlayerState());
		if(!playerState)
		{
			return;
		}

		utils::UtilsLibrary::SendGameplayEventWithTargetData<FTargetDataPickupObject>(
			character,
			FGameplayTag::RequestGameplayTag(TAG_EVENT_PICKUP_INDICATOR),
			this,
			new FTargetDataPickupObject(GetActorLocation(), this));
		
		//Si enviamos un efecto también funciona, incluso podemos dejar la habilidad como Local Only,
		//pero no podemos pasar parámetros
		//mPickupIndicatorEffectHandle = playerState->GetAbilitySystemComponentInterface()->ApplyGameplayEffectToSelf(PickupIndicatorEffect);
	}
}

void APickupObjectBase::OnSphereEndOverlapServer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//No matter the state of the weapon if end overlap, remove pickup indicator.
	//imagin situation where player 1 is seeing its player indicator, and a player 2 equips the
	//object the player 1 is seeing, we want the indicator be disabled in both.
	
	if(const auto character = Cast<ACharacterBase>(OtherActor))
	{
		const auto playerState = Cast<APlayerStateBase>(character->GetPlayerState());
		if(!playerState)
		{
			return;
		}
		//Remove the ability with tag pickup indicator
		FGameplayTagContainer cancelTags;
		UBlueprintGameplayTagLibrary::AddGameplayTag(cancelTags, FGameplayTag::RequestGameplayTag(TAG_ABILITY_PICKUP_INDICATOR));
		playerState->GetAbilitySystemComponent()->CancelAbilities(&cancelTags);

		//En caso que enviemos effecto hay que quitarlo
		//playerState->GetAbilitySystemComponentInterface()->RemoveGameplayEffect(mPickupIndicatorEffectHandle);
	}
}
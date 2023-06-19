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

// Sets default values
APickupObjectBase::APickupObjectBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

void APickupObjectBase::ChangeStateServer(EPickupObjectState state)
{
	if(!HasAuthority())
	{
		return;
	}
	
	State = state;
	switch (State)
	{
	case EPickupObjectState::Equipped:
		DisableDetectionArea();
		break;
	case EPickupObjectState::Dropped:
		EnableDetectionArea();
		break;
	case EPickupObjectState::Initial:
		break;
	default:
		break;
	}
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

FVector APickupObjectBase::GetPickupWidgetLocation(const FBoxSphereBounds& bounds) const
{
	const auto height = bounds.BoxExtent.Z * 2.0f; //Extend is half size side
	const auto objectLocation = GetActorLocation();
	//DrawDebugBox(GetWorld(), bounds.Origin + GetActorLocation() , bounds.BoxExtent,FColor::Green, false, 20);
	//DrawDebugSphere(GetWorld(), bounds.Origin + GetActorLocation() + FVector(0.0f, 0.0f, height), 10, 30, FColor::Red, false, 20);
	return bounds.Origin + GetActorLocation() + FVector(0.0f, 0.0f, height);
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

		check(character->GetMesh());
		utils::UtilsLibrary::SendGameplayEventWithTargetData<FTargetDataPickupObject>(
			character,
			FGameplayTag::RequestGameplayTag(TAG_EVENT_PICKUP_INDICATOR),
			this,
			new FTargetDataPickupObject(GetPickupWidgetLocation(Mesh->GetLocalBounds()), this));

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
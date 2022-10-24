// Fill out your copyright notice in the Description page of Project Settings.


#include "PickableObjectBase.h"

#include "BlueprintGameplayTagLibrary.h"
#include "BattleRoyale/core/Abilities/GameplayTagsList.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameMode/PlayerState/PlayerStateBase.h"
#include "BattleRoyale/core/Utils/UtilsLibrary.h"
#include "BattleRoyale/core/Utils/TargetDatas/TargetDataPickupIndicator.h"
#include "BattleRoyale/BattleRoyale.h"

// Sets default values
APickableObjectBase::APickableObjectBase()
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
	AreaSphere->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void APickableObjectBase::BeginPlay()
{
	Super::BeginPlay();
	
	if(HasAuthority())
	{
		//Enable the Area Sphere 
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlapServer);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnSphereEndOverlapServer);
	}
}

FVector APickableObjectBase::GetPickupWidgetLocation(const FBoxSphereBounds& bounds) const
{
	const auto height = bounds.BoxExtent.Z * 2.0f; //Extend is half size side
	const auto objectLocation = GetActorLocation();
	//DrawDebugBox(GetWorld(), bounds.Origin + GetActorLocation() , bounds.BoxExtent,FColor::Green, false, 20);
	//DrawDebugSphere(GetWorld(), bounds.Origin + GetActorLocation() + FVector(0.0f, 0.0f, height), 10, 30, FColor::Red, false, 20);
	return bounds.Origin + GetActorLocation() + FVector(0.0f, 0.0f, height);
}

void APickableObjectBase::OnSphereOverlapServer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(const auto character = Cast<ACharacterBase>(OtherActor))
	{
		const auto playerState = Cast<APlayerStateBase>(character->GetPlayerState());
		if(!playerState)
		{
			return;
		}

		check(character->GetMesh());
		utils::UtilsLibrary::SendGameplayEventWithTargetData<FTargetDataPickupIndicator>(
			character,
			FGameplayTag::RequestGameplayTag(TAG_EVENT_PICKUP_INDICATOR),
			new FTargetDataPickupIndicator(GetPickupWidgetLocation(Mesh->GetLocalBounds()), this));

		//Si enviamos un efecto también funciona, incluso podemos dejar la habilidad como Local Only,
		//pero no podemos pasar parámetros
		//mPickupIndicatorEffectHandle = playerState->GetAbilitySystemComponentInterface()->ApplyGameplayEffectToSelf(PickupIndicatorEffect);
	}
}

void APickableObjectBase::OnSphereEndOverlapServer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
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

bool APickableObjectBase::AttachToComponent(USkeletalMeshComponent* meshComponent, const FAttachmentTransformRules& attachmentRules, const FName& socketName)
{
	if(Mesh == nullptr)
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][ACharacterBase::AttachToComponent] weapon has no mesh"), *GetName());
		return false;
	}
	
	return Mesh->AttachToComponent(meshComponent, attachmentRules, socketName);
}

void APickableObjectBase::DetachFromComponent(const FDetachmentTransformRules& rules)
{
	if(Mesh == nullptr)
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][ACharacterBase::DeattachToComponent] weapon has no mesh"), *GetName());
		return;
	}
	
	Mesh->DetachFromComponent(rules);
}
#include "FirstPersonCharacter.h"

#include "BattleRoyale/core/Utils/GameplayBlueprintFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"

AFirstPersonCharacter::AFirstPersonCharacter()
{
	// Create a CameraComponent	
	mFirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	mFirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	mFirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	mFirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	/*mCharacterMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	mCharacterMesh1P->SetOnlyOwnerSee(true);
	mCharacterMesh1P->SetupAttachment(mFirstPersonCameraComponent);
	mCharacterMesh1P->bCastDynamicShadow = false;
	mCharacterMesh1P->CastShadow = false;
	mCharacterMesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	mCharacterMesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));*/
}

void AFirstPersonCharacter::HideMesh() const
{
	if(!IsLocallyControlled())
	{
		return;
	}

	GetMesh()->SetHiddenInGame(true, true);
}

void AFirstPersonCharacter::ConvertIntoMesh1P() const
{
	GetMesh()->HideBoneByName(ThinghRightBoneName, EPhysBodyOp::PBO_None);
	GetMesh()->HideBoneByName(ThinghLeftBoneName, EPhysBodyOp::PBO_None);
	GetMesh()->HideBoneByName(NeckBoneName, EPhysBodyOp::PBO_None);
	GetMesh()->CastShadow = false;
	
	//GetMesh()->HideBoneByName(FName("Hips"), EPhysBodyOp::PBO_None);
	//GetMesh()->UnHideBoneByName("LeftShoulder");
	//GetMesh()->UnHideBoneByName("RightShoulder");
}

void AFirstPersonCharacter::ChangeCharacterMesh1PColor(const FColor& color)
{
	/*
	if(mCharacterMesh1PMaterial == nullptr)
	{
		mCharacterMesh1PMaterial = UGameplayBlueprintFunctionLibrary::CreateAndAssignMaterialInstanceDynamicToMesh(mCharacterMesh1P);
	}

	mCharacterMesh1PMaterial->SetVectorParameterValue("BodyColor", color);*/
}

void AFirstPersonCharacter::DoInitialize(bool isLocallyControlled)
{
	if(IsLocallyControlled())
	{
		//HideMesh();
		ConvertIntoMesh1P();
	}
}

void AFirstPersonCharacter::DoDieClient()
{
	HideMesh();
}

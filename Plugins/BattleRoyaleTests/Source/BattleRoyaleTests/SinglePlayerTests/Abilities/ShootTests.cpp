#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "WeaponMock.h"
#include "AssetRegistry/AssetRegistryModule.h"

#include "BattleRoyale/core/Abilities/AbilitiesInput.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameMode/PlayerState/PlayerStateBase.h"
#include "BattleRoyale/core/PickableObjects/Weapons/IWeapon.h"
#include "BattleRoyale/core/PickableObjects/Weapons/WeaponBase.h"

#include "BattleRoyaleTests/TestUtils.h"
#include "Engine/ObjectLibrary.h"
#include "Kismet/GameplayStatics.h"

#if WITH_EDITOR
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FShootProjectileGunTest_When_Spawn_Then_CanNotShoot,
                                 "BattleRoyale.SinglePlayer.Abilities.ShootProjectileGunAbility.When_Spawn_Then_CanNotShoot",
                                 EAutomationTestFlags::ApplicationContextMask  | EAutomationTestFlags::ProductFilter)

bool FShootProjectileGunTest_When_Spawn_Then_CanNotShoot::RunTest(const FString& Parameters)
{

	UWorld* world = FAutomationEditorCommonUtils::CreateNewMap(); 
	if (!world)
	{
		return false;
	}
	
	const auto character = world->SpawnActor<ACharacterBase>();
	
	//For more tests check AutomationTest.h (lines 1347 - 1639)
	TestNotNull<ACharacterBase>(TEXT("Testing if there is a player actor in level"), character);
	
	TestFalse(TEXT("When Spawning New Character Can NOT Shoot"), character->CanShoot());

	character->Destroy();

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FShootProjectileGunTest_When_NoWeapon_Then_CanNotShoot,
								 "BattleRoyale.SinglePlayer.Abilities.ShootProjectileGunAbility.When_NoWeapon_Then_CanNotShoot",
								 EAutomationTestFlags::ApplicationContextMask  | EAutomationTestFlags::ProductFilter)

bool FShootProjectileGunTest_When_NoWeapon_Then_CanNotShoot::RunTest(const FString& Parameters)
{

	UWorld* world = FAutomationEditorCommonUtils::CreateNewMap(); 
	if (!world)
	{
		return false;
	}
	
	const auto character = world->SpawnActor<ACharacterBase>();
	
	//For more tests check AutomationTest.h (lines 1347 - 1639)
	TestNotNull<ACharacterBase>(TEXT("Testing if there is a player actor in level"), character);

	const auto gunComponent = character->GetGunComponent();
	check(gunComponent);
	
	TestFalse(TEXT("When No Weapon"), gunComponent->HasWeaponEquipped());
	TestFalse(TEXT("When No Weapon Can NOT Shoot"), character->CanShoot());

	character->Destroy();

	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FShootProjectileGunTest_When_WeaponButNoAmmo_Then_CanNotShoot,
								 "BattleRoyale.SinglePlayer.Abilities.ShootProjectileGunAbility.When_WeaponButNoAmmo_Then_CanNotShoot",
								 EAutomationTestFlags::ApplicationContextMask  | EAutomationTestFlags::ProductFilter)

bool FShootProjectileGunTest_When_WeaponButNoAmmo_Then_CanNotShoot::RunTest(const FString& Parameters)
{

	const FString mapName = TEXT("/Game/Maps/SampleTest.SampleTest");
	if (!AutomationOpenMap(mapName))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load world"));
		return false;
	}
 
	UWorld *world = GetAnyGameWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to retrieve world"));
		return false;
	}
	
	//ADD_LATENT_AUTOMATION_COMMAND(FMyCommand(gameState));
	//ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(5.0f));
	//while(!gameState->IsGameReadyToStart()){};
	
	const auto character = UGameplayStatics::GetPlayerCharacter(world, 0);
	if(!character)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to retrieve character"));
		return false;
	}
	const auto characterBase = Cast<ACharacterBase>(character);
	//For more tests check AutomationTest.h (lines 1347 - 1639)
	TestNotNull<ACharacterBase>(TEXT("Testing if there is a player actor in level"), characterBase);
/*
	//LOAD ASSET
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	// Create a filter to specify the directory
	FARFilter AssetFilter;
	AssetFilter.PackagePaths.Add(FName("/Game/Core/Blueprints/Weapons/AssaultRifle/"));

	// Query the assets in the directory
	TArray<FAssetData> AssetData;
	AssetRegistryModule.Get().GetAssets(AssetFilter, AssetData);  
	
	const auto blueprintWeaponAsset = Cast<UBlueprint>(AssetData[0].GetAsset());	
	const auto pickableObject = blueprintWeaponAsset->GeneratedClass;
	const auto weapon = pickableObject->GetDefaultObject<AWeaponBase>();*/

	const auto weapon = NewObject<AWeaponMock>();
	
	const auto inventoryComponent = characterBase->GetInventoryComponent();
	inventoryComponent->PickupObjectServer(weapon);
	
	const auto gunComponent = characterBase->GetGunComponent();
	check(gunComponent);

	const auto hasWeaponEquipped = gunComponent->HasWeaponEquipped();
	TestTrue(TEXT("When Weapon"), hasWeaponEquipped);
	if(!hasWeaponEquipped)
	{
		character->Destroy();
		return false;
	}
	
	TestFalse(TEXT("When No Ammo"), gunComponent->GetEquippedWeapon()->HasAmmo());
	TestFalse(TEXT("When Weapon but No Ammo Can NOT Shoot"), characterBase->CanShoot());
	
	character->Destroy();

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FShootProjectileGunTest_When_AbilityShootProjectileGunIsTriggered_And_HaveAWeaponWithAmmo_CharacterIsShooting,
								 "BattleRoyale.SinglePlayer.Abilities.ShootProjectileGunAbility.When_AbilityShootProjectileGunIsTriggered_And_HaveAWeaponWithAmmo_CharacterIsShooting",
								 EAutomationTestFlags::ApplicationContextMask  | EAutomationTestFlags::ProductFilter)

bool FShootProjectileGunTest_When_AbilityShootProjectileGunIsTriggered_And_HaveAWeaponWithAmmo_CharacterIsShooting::RunTest(const FString& Parameters)
{
	const FString mapName = TEXT("/Game/Maps/SampleTest.SampleTest");
	if (!AutomationOpenMap(mapName))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load world"));
		return false;
	}
 
	UWorld *world = GetAnyGameWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to retrieve world"));
		return false;
	}

	//const auto gameState = Cast<IIGameState>(world->GetGameState());
	
	//while(!gameState->DidCountdownFinish()){};
	
	const auto playerController = world->GetFirstPlayerController();
	const auto character = UGameplayStatics::GetPlayerCharacter(world, 0);
	if(!character)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to retrieve character"));
		return false;
	}
	const auto characterBase = Cast<ACharacterBase>(character);
	//For more tests check AutomationTest.h (lines 1347 - 1639)
	TestNotNull<ACharacterBase>(TEXT("Testing if there is a player actor in level"), characterBase);

	const auto characterMovementComponent = characterBase->GetCharacterMovement();
	
	characterMovementComponent->Velocity = FVector(100, 0, 0);
	characterMovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
	const auto maxWalkSpeedBeforeSprint = characterMovementComponent->MaxWalkSpeed;

	//Enable ability simulating key left shift (sprint) pressed.
	const auto playerState = Cast<APlayerStateBase>(characterBase->GetPlayerState());
	const auto gas = playerState->GetAbilitySystemComponent();

	gas->AbilityLocalInputPressed(static_cast<int32>(EAbilityInputID::Sprint));
	TestTrue(TEXT("When Character is sprinting SPRINT max walk speed increased"),characterMovementComponent->MaxWalkSpeed > maxWalkSpeedBeforeSprint);

	//world->DestroyWorld(true);
	
	return true;
}
#endif
#endif
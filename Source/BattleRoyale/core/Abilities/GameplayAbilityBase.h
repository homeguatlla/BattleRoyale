
#pragma once

#include "CoreMinimal.h"
#include "AbilitiesInput.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbilityBase.generated.h"

class IICharacter;
/**
 * 
 */
UCLASS()
class UGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGameplayAbilityBase();
	
	// If an ability is marked as 'ActivateAbilityOnGranted', activate them immediately when given here
	// Epic's comment: Projects may want to initiate passives or do other "BeginPlay" type of logic here.
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* actorInfo, const FGameplayAbilitySpec& spec) override;

	IICharacter* GetCharacter(const FGameplayAbilityActorInfo* actorInfo) const;
	
	// Abilities with this set will automatically activate when the input is pressed
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EAbilityInputID AbilityInputID = EAbilityInputID::None;

	// Value to associate an ability with an slot without tying it to an automatically activated input.
	// Passive abilities won't be tied to an input so we need a way to generically associate abilities with slots.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EAbilityInputID AbilityID = EAbilityInputID::None;

	// Tells an ability to activate immediately when its granted. Used for passive abilities and abilities forced on others.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	bool ActivateAbilityOnGranted = false;
};

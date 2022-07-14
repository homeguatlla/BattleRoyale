// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "BattleRoyale/core/Character/AttributeSetHealth.h"
#include "Components/ActorComponent.h"
#include "HurtComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLEROYALE_API UHurtComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHurtComponent();

	void Initialize();
	bool RegisterToHealthAttributeDelegate(std::function<void(const FOnAttributeChangeData& data)> callback) const;
	void SetInvulnerableServer(bool isInvulnerable);
	
	//TODO remove it
	void TakeDamageServer(float damage, APlayerController* instigator, APlayerController* hurt);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Hurt")
	TSubclassOf<UGameplayEffect> InitializeHurtEffect;

private:
	// Implement IAbilitySystemInterface
	virtual class IIAbilitySystemInterfaceBase* GetAbilitySystemComponent() const;

	UFUNCTION(Server, Reliable)
	void ServerSetInvulnerable(bool isInvulnerable);
	
	UPROPERTY()
	UAttributeSetHealth* mHealthAttributes;
	
	FActiveGameplayEffectHandle mVulnerableEffectHandle;

	UPROPERTY(EditAnywhere, Category = "Hurt")
	TSubclassOf<UGameplayEffect> VulnerableEffect;
};

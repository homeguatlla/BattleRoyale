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

	void InitializeServer() const;
	bool RegisterToHealthAttributeDelegate(std::function<void(const FOnAttributeChangeData& data)> callback) const;
	void SetInvulnerableServer(bool isInvulnerable);
	virtual bool IsAlive() const { return GetCurrentHealth() > 0.0f; }
	float GetCurrentHealth() const;
	bool IsReady() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Hurt")
	TSubclassOf<UGameplayEffect> InitializeHurtEffect;
	
private:
	// Implement IAbilitySystemInterface
	virtual class IIAbilitySystemInterfaceBase* GetAbilitySystemComponent() const;

	UFUNCTION(Server, Reliable)
	void ServerSetInvulnerable(bool isInvulnerable);
	
	FActiveGameplayEffectHandle mInvulnerableEffectHandle;

	UPROPERTY(EditAnywhere, Category = "Hurt")
	TSubclassOf<UGameplayEffect> InvulnerableEffect;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API APlayerControllerBase : public APlayerController, public IIPlayerController
{

	GENERATED_BODY()

public:
	virtual void EnableInput(bool enable) override;
	float GetServerTime() const;
	virtual void ReceivedPlayer() override; //Sync with server clock as soon as possible.

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	
	UFUNCTION(Client, Reliable)
	void ClientEnableInput(bool enable);

	/**
	 * Sync time between Client and Server
	 */
	
	//Request the current server time, passing in the client's time when the request was sent
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float timeOfClientRequest);

	//Reports the current server time to the client in response to ServerRequestServerTime
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float timeOfClientRequest, float timeServerReceivedClientRequest);
	void CheckTimeSync(float DeltaSeconds);
	void CheckConnectivity() const;
	
	float mClientServerDelta = 0.0f; //Difference between Client and Server time.

	UPROPERTY(EditAnywhere, Category = Time)
	float mTimeSyncFrequency = 5.0f;

	float mTimeSyncRunningTime = 0.0f;
};

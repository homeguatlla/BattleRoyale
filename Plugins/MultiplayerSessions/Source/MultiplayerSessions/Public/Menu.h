// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Menu.generated.h"

const FString MATCH_TYPE_FREE_FOR_ALL(TEXT("FreeForAll"));

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void Setup(
		int numberPublicConnections = 4,
		const FString& matchType = FString(TEXT("FreeForAll")),
		const FString& lobbyPath = FString(TEXT("/Game/Maps/TestMultiplayerSessions/TestMultiplayerSessionsLobby")));

protected:
	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;
	
private:
	void SetInputMode();
	void SaveMultiplayerSessionsSubsystem();
	void TearDown();
	void SubscribeToMultiplayerSessionDelegates();
	
	UFUNCTION()
	void HostButtonClicked();
	UFUNCTION()
	void JoinButtonClicked();

	// Callbacks for the custom delegates on the MultiplayerSessionsSubsystem
	UFUNCTION()
	void OnCreateSession(bool wasSuccessful);
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& sessionsResults, bool wasSucccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type result);
	UFUNCTION()
	void OnDestroySession(bool wasSuccessful);
	UFUNCTION()
	void OnStartSession(bool wasSuccessful);
	
	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton; //the exact same name than the button name in the widget
	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	//The subsystem designed to handle all online session functionality
	class UMultiplayerSessionsSubsystem* mMultiplayerSessionsSubsystem;

	int mNumPublicConnections {4};
	FString mMatchType{MATCH_TYPE_FREE_FOR_ALL};
	FString mPathToLobby;
};

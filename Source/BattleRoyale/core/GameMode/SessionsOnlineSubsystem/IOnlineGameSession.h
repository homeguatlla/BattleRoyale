#pragma once

#include "CoreMinimal.h"


#include "OnlineSessionSettings.h"
#include "UObject/Interface.h"
#include "IOnlineGameSession.generated.h"


USTRUCT(BlueprintType)
struct FOnlineSessionFindResult
{
	GENERATED_BODY()

	FOnlineSessionFindResult() = default;
	FOnlineSessionFindResult(
		const FString& sessionIdParam,
		const FString& serverNameParam,
		int32 numOpenPublicConnectionsParam,
		int32 pingInMsParam) :
	sessionId(sessionIdParam),
	serverName(serverNameParam),
	numOpenPublicConnections(numOpenPublicConnectionsParam),
	pingInMs(pingInMsParam) {}

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Online Session Find Result")
	FString sessionId;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Online Session Find Result")
	FString serverName;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Online Session Find Result")
	int32 numOpenPublicConnections;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Online Session Find Result")
	int32 pingInMs;
};

UINTERFACE()
class UOnlineGameSession : public UInterface
{
	GENERATED_BODY()
};

class IOnlineGameSession
{
	GENERATED_BODY()
	
public:
    virtual void CreateSession(bool isLAN) = 0;
	virtual void StartSession() = 0;
	virtual void EndSession() = 0;
	virtual void FindSessions() = 0;
	virtual bool JoinSession(const FString& sessionId) = 0;
	virtual void DestroySessionAndLeaveGame() = 0;
	virtual TArray<FOnlineSessionFindResult> GetAvailableSessions() const = 0;
	virtual bool HasCurrentSession() const = 0;
	virtual FString GetCurrentSessionIdStr() const = 0;
	virtual uint8 GetFindSessionsStatus() const = 0;

	virtual bool HasLobbyMap() const = 0;
	virtual void StartGame() = 0;
};

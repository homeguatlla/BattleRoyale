#pragma once
#include "MultiplayerConfigurationInfo.generated.h"

UCLASS(BlueprintType)
class UMultiplayerConfigurationInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	FName GetGameMapName() const { return GameMapName; }
	FName GetLobbyMapName() const { return LobbyMapName; }
	FName GetMainMapName() const { return MainMapName; }
	FName GetMapsPath() const { return MapsPath; }
	uint8 GetMaxNumPlayers() const { return MaxNumPlayers; }
	
private:
	UPROPERTY(EditAnywhere, Category="Multiplayer")
	uint8 MaxNumPlayers = 4;
	
	UPROPERTY(EditAnywhere, Category="Travelling")
	FName LobbyMapName;
	UPROPERTY(EditAnywhere, Category="Travelling")
	FName MainMapName;
	UPROPERTY(EditAnywhere, Category="Travelling")
	FName GameMapName;
	UPROPERTY(EditAnywhere, Category="Travelling")
	FName MapsPath;
};

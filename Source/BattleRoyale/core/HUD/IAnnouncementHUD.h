#pragma once

#include "IAnnouncementHUD.generated.h"

UINTERFACE(Blueprintable)
class BATTLEROYALE_API UAnnouncementHUD : public UInterface
{
	GENERATED_BODY()
};

class IAnnouncementHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Announcement HUD")
	void OnAnnounceNewDeath(const FString& killerName, const FString& victimName);
};
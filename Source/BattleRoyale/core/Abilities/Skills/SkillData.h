#pragma once
#include "CoreMinimal.h"
#include "BattleRoyale/core/HUD/Widgets/Skills/SkillWidget.h"
#include "SkillData.generated.h"

USTRUCT(BlueprintType)
struct BATTLEROYALE_API FSkillData
{
	GENERATED_USTRUCT_BODY()
	
	FSkillData() {}
	FSkillData(const TSubclassOf<USkillWidget>& userWidget, const FGameplayTag& tag, float cooldown) : WidgetHUD(userWidget), Tag(tag), Cooldown(cooldown) {}

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag Tag;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<USkillWidget> WidgetHUD;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float Cooldown {0.0f};
};
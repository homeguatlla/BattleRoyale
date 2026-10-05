#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VisorHealthWidget.generated.h"

class UBorder;
class UTextBlock;
class UProgressBar;

/**
 * Prototype of the new helmet-hologram health module.
 * The widget tree is built in code so it can be tested without any Widget Blueprint.
 */
UCLASS()
class BATTLEROYALE_API UVisorHealthWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealth(float health);
	void SetAccentColor(const FLinearColor& accent);

	UPROPERTY(EditAnywhere, Category = "Visor")
	float MaxHealth = 100.0f;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	void BuildTree();
	void ApplyStyle();

	UFUNCTION()
	void OnRefreshHealth(float health);

	UPROPERTY()
	UBorder* mGlowBorder = nullptr;
	UPROPERTY()
	UBorder* mPanelBorder = nullptr;
	UPROPERTY()
	UBorder* mBadgeBorder = nullptr;
	UPROPERTY()
	UTextBlock* mNameText = nullptr;
	UPROPERTY()
	UTextBlock* mValueText = nullptr;
	UPROPERTY()
	UProgressBar* mHealthBar = nullptr;

	FLinearColor mAccent = FLinearColor::FromSRGBColor(FColor(0x38, 0xE1, 0xFF));
	float mHealth = 100.0f;
};

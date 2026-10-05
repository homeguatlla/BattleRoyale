#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VisorHealthWidget.generated.h"

class UBorder;
class UScaleBox;
class USizeBox;
class UTextBlock;
class UProgressBar;

/**
 * Prototype of the new helmet-hologram health module.
 * The widget tree is built in code so it can be tested without any Widget Blueprint.
 *
 * The panel gets louder as health drops: blue (healthy) -> amber (hurt) -> red (critical, pulsing,
 * CRÍTICA tag), and flashes every time damage is taken.
 */
UCLASS()
class BATTLEROYALE_API UVisorHealthWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealth(float health);
	void SetAccentColor(const FLinearColor& accent);
	// Lays the widget out at logicalSize and renders it scale times bigger, so a render target
	// of logicalSize * scale is supersampled when it is projected on the visor.
	void SetSupersample(float scale, const FVector2D& logicalSize);

	UPROPERTY(EditAnywhere, Category = "Visor")
	float MaxHealth = 100.0f;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	void BuildTree();
	void ApplyStyle();
	void TickVisuals();

	UFUNCTION()
	void OnRefreshHealth(float health);

	UPROPERTY()
	UScaleBox* mScaleBox = nullptr;
	UPROPERTY()
	USizeBox* mLogicalSizeBox = nullptr;
	UPROPERTY()
	UBorder* mGlowBorder = nullptr;
	UPROPERTY()
	UBorder* mPanelBorder = nullptr;
	UPROPERTY()
	UBorder* mBadgeBorder = nullptr;
	UPROPERTY()
	UBorder* mCriticalTag = nullptr;
	UPROPERTY()
	UTextBlock* mNameText = nullptr;
	UPROPERTY()
	UTextBlock* mValueText = nullptr;
	UPROPERTY()
	UProgressBar* mHealthBar = nullptr;

	FLinearColor mAccent = FLinearColor::FromSRGBColor(FColor(0x38, 0xE1, 0xFF));
	float mHealth = 100.0f;
	float mSupersample = 1.0f;
	FVector2D mLogicalSize = FVector2D(1920.0f, 1080.0f);

	FTimerHandle mVisualsTimer;
	float mVisualsTime = 0.0f;
	float mDamageFlash = 0.0f;
};

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VisorPrototype.generated.h"

class URetainerBox;
class UWidgetComponent;
class UVisorHealthWidget;

/**
 * Technique B: the 2D health module rendered through a RetainerBox whose effect material
 * bends it like a curved visor (M_VisorWarp, created by Tools/Visor/create_visor_material.py).
 */
UCLASS()
class BATTLEROYALE_API UVisorWarpWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetWarp(float curvature, float scanlines, float aberration);
	void SetHealth(float health);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	UPROPERTY()
	URetainerBox* mRetainer = nullptr;
	UPROPERTY()
	UVisorHealthWidget* mHealthWidget = nullptr;
};

/**
 * Prototype to compare the two visor techniques in game. Driven from console commands
 * declared in ABattleRoyaleHUD:
 *   VisorPrototype 0|1|2         0 = off, 1 = 3D cylinder in front of the camera, 2 = 2D + warp material
 *   VisorTune <arcDeg> <distance> cylinder arc and distance to the camera (technique 1)
 *   VisorWarp <curv> <scan> <aberr> warp material parameters (technique 2)
 *   VisorHealth <value>          fake a health value to check the critical state
 */
UCLASS()
class BATTLEROYALE_API UVisorPrototype : public UObject
{
	GENERATED_BODY()

public:
	void SetMode(APlayerController* playerController, int32 mode);
	void Tune(float arcAngle, float distance);
	void SetWarp(float curvature, float scanlines, float aberration);
	void SetHealth(float health);

private:
	void Clear();
	void Create3D(APlayerController* playerController);
	void Create2D(APlayerController* playerController);
	void Layout3D() const;

	UPROPERTY()
	UWidgetComponent* mWidgetComponent = nullptr;
	UPROPERTY()
	UVisorWarpWidget* mWarpWidget = nullptr;

	float mArcAngle = 45.0f;
	float mDistance = 40.0f;
	float mCurvature = 0.12f;
	float mScanlines = 0.06f;
	float mAberration = 0.0015f;
};

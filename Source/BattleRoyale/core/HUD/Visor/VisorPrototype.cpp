#include "VisorPrototype.h"

#include "VisorHealthWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Camera/CameraComponent.h"
#include "Components/RetainerBox.h"
#include "Components/WidgetComponent.h"
#include "Engine/GameViewportClient.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

namespace
{
	const TCHAR* VISOR_WARP_MATERIAL = TEXT("/Game/Core/UI/Visor/M_VisorWarp.M_VisorWarp");
	// Slate-drawn HUD re-projected on a cylinder: same look as the 3D visor with exact colours.
	const TCHAR* VISOR_CYLINDER_MATERIAL = TEXT("/Game/Core/UI/Visor/M_VisorCylinder.M_VisorCylinder");
	// Drawn after TSR/TAA and without depth test: stops the jitter and the clipping into walls.
	const TCHAR* VISOR_WIDGET_MATERIAL = TEXT("/Game/Core/UI/Visor/M_VisorWidget.M_VisorWidget");
	// Render target resolution relative to the screen; >1 supersamples the projected visor.
	const float SUPERSAMPLE = 2.0f;

	FVector2D GetViewportSize()
	{
		FVector2D size(1920.0f, 1080.0f);
		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(size);
		}
		return size;
	}
}

TSharedRef<SWidget> UVisorWarpWidget::RebuildWidget()
{
	if (!WidgetTree)
	{
		WidgetTree = NewObject<UWidgetTree>(this, TEXT("WidgetTree"));
	}
	if (!WidgetTree->RootWidget)
	{
		mRetainer = WidgetTree->ConstructWidget<URetainerBox>(URetainerBox::StaticClass(), TEXT("VisorRetainer"));
		WidgetTree->RootWidget = mRetainer;

		mHealthWidget = CreateWidget<UVisorHealthWidget>(GetOwningPlayer(), UVisorHealthWidget::StaticClass());
		// On screen the widget is laid out in DPI-scaled units, no supersampling needed.
		const float dpiScale = UWidgetLayoutLibrary::GetViewportScale(this);
		mHealthWidget->SetSupersample(1.0f, GetViewportSize() / FMath::Max(dpiScale, 0.01f));
		mRetainer->AddChild(mHealthWidget);

		const FString materialPath = mMaterialPath.IsEmpty() ? FString(VISOR_WARP_MATERIAL) : mMaterialPath;
		if (UMaterialInterface* material = LoadObject<UMaterialInterface>(nullptr, *materialPath))
		{
			mRetainer->SetEffectMaterial(material);
			mRetainer->SetTextureParameter(TEXT("Texture"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UVisorWarpWidget: %s not found, run the matching script in Tools/Visor from the editor"), *materialPath);
		}
	}
	return Super::RebuildWidget();
}

void UVisorWarpWidget::SetWarp(float curvature, float scanlines, float aberration)
{
	UMaterialInstanceDynamic* material = mRetainer ? mRetainer->GetEffectMaterial() : nullptr;
	if (material)
	{
		material->SetScalarParameterValue(TEXT("Curvature"), curvature);
		material->SetScalarParameterValue(TEXT("Scanlines"), scanlines);
		material->SetScalarParameterValue(TEXT("Aberration"), aberration);
	}
}

void UVisorWarpWidget::SetCylinder(float tanHalfFov, float aspect, float arcAngleRadians, float glowStrength, float glowRadius)
{
	UMaterialInstanceDynamic* material = mRetainer ? mRetainer->GetEffectMaterial() : nullptr;
	if (material)
	{
		material->SetScalarParameterValue(TEXT("TanHalfFov"), tanHalfFov);
		material->SetScalarParameterValue(TEXT("Aspect"), aspect);
		material->SetScalarParameterValue(TEXT("ArcAngle"), arcAngleRadians);
		material->SetScalarParameterValue(TEXT("GlowStrength"), glowStrength);
		material->SetScalarParameterValue(TEXT("GlowRadius"), glowRadius);
	}
}

void UVisorWarpWidget::SetHealth(float health)
{
	if (mHealthWidget)
	{
		mHealthWidget->SetHealth(health);
	}
}

void UVisorPrototype::SetMode(APlayerController* playerController, int32 mode)
{
	Clear();
	if (!playerController)
	{
		return;
	}
	SetLegacyHUDHidden(playerController, mode != 0);

	if (mode == 1)
	{
		Create3D(playerController);
	}
	else if (mode == 2)
	{
		Create2D(playerController);
	}
	else if (mode == 3)
	{
		CreateCylinder2D(playerController);
	}
}

void UVisorPrototype::SetLegacyHUDHidden(APlayerController* playerController, bool hidden)
{
	for (UUserWidget* widget : mHiddenLegacyWidgets)
	{
		if (widget)
		{
			widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
	mHiddenLegacyWidgets.Reset();

	if (!hidden)
	{
		return;
	}

	TArray<UUserWidget*> widgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(playerController, widgets, UUserWidget::StaticClass(), true);
	for (UUserWidget* widget : widgets)
	{
		if (widget->GetClass()->GetName().Contains(TEXT("CharacterHUD")) && widget->IsVisible())
		{
			widget->SetVisibility(ESlateVisibility::Collapsed);
			mHiddenLegacyWidgets.Add(widget);
		}
	}
}

void UVisorPrototype::Clear()
{
	if (mWidgetComponent)
	{
		mWidgetComponent->DestroyComponent();
		mWidgetComponent = nullptr;
	}
	if (mWarpWidget)
	{
		mWarpWidget->RemoveFromParent();
		mWarpWidget = nullptr;
	}
	mIsCylinder2D = false;
}

void UVisorPrototype::Create3D(APlayerController* playerController)
{
	APawn* pawn = playerController->GetPawn();
	UCameraComponent* camera = pawn ? pawn->FindComponentByClass<UCameraComponent>() : nullptr;
	if (!camera)
	{
		UE_LOG(LogTemp, Warning, TEXT("UVisorPrototype: no pawn camera to attach the visor to"));
		return;
	}

	const FVector2D viewport = GetViewportSize();

	mWidgetComponent = NewObject<UWidgetComponent>(pawn, TEXT("VisorWidgetComponent"));
	mWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	mWidgetComponent->SetWidgetClass(UVisorHealthWidget::StaticClass());
	mWidgetComponent->SetDrawSize(viewport * SUPERSAMPLE);
	mWidgetComponent->SetGeometryMode(EWidgetGeometryMode::Cylinder);
	mWidgetComponent->SetCylinderArcAngle(mArcAngle);
	mWidgetComponent->SetBlendMode(EWidgetBlendMode::Transparent);
	mWidgetComponent->SetTwoSided(false);
	mWidgetComponent->SetCastShadow(false);
	mWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mWidgetComponent->SetOnlyOwnerSee(true);
	mWidgetComponent->SetOwnerPlayer(playerController->GetLocalPlayer());
	mWidgetComponent->SetupAttachment(camera);
	mWidgetComponent->RegisterComponent();

	if (UMaterialInterface* material = LoadObject<UMaterialInterface>(nullptr, VISOR_WIDGET_MATERIAL))
	{
		mWidgetComponent->SetMaterial(0, material);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UVisorPrototype: %s not found, run Tools/Visor/create_visor_widget_material.py in the editor"), VISOR_WIDGET_MATERIAL);
	}

	if (UVisorHealthWidget* widget = Cast<UVisorHealthWidget>(mWidgetComponent->GetUserWidgetObject()))
	{
		widget->SetSupersample(SUPERSAMPLE, viewport);
	}

	Layout3D();
}

void UVisorPrototype::Layout3D() const
{
	if (!mWidgetComponent)
	{
		return;
	}

	const UCameraComponent* camera = Cast<UCameraComponent>(mWidgetComponent->GetAttachParent());
	const float fov = camera ? camera->FieldOfView : 90.0f;
	const float visibleWidth = 2.0f * mDistance * FMath::Tan(FMath::DegreesToRadians(fov * 0.5f));
	const float scale = visibleWidth / mWidgetComponent->GetDrawSize().X;

	mWidgetComponent->SetCylinderArcAngle(mArcAngle);
	// The widget's front faces +X, so turn it around to face the camera.
	mWidgetComponent->SetRelativeLocationAndRotation(FVector(mDistance, 0.0f, 0.0f), FRotator(0.0f, 180.0f, 0.0f));
	mWidgetComponent->SetRelativeScale3D(FVector(scale));
}

void UVisorPrototype::Create2D(APlayerController* playerController)
{
	mWarpWidget = CreateWidget<UVisorWarpWidget>(playerController, UVisorWarpWidget::StaticClass());
	mWarpWidget->AddToViewport(10);
	mWarpWidget->SetWarp(mCurvature, mScanlines, mAberration);
}

void UVisorPrototype::CreateCylinder2D(APlayerController* playerController)
{
	const APawn* pawn = playerController->GetPawn();
	const UCameraComponent* camera = pawn ? pawn->FindComponentByClass<UCameraComponent>() : nullptr;
	// The visor is part of the helmet: it keeps the base FOV even if the weapon zooms in later.
	mCylinderFov = camera ? camera->FieldOfView : 90.0f;

	mWarpWidget = CreateWidget<UVisorWarpWidget>(playerController, UVisorWarpWidget::StaticClass());
	mWarpWidget->SetMaterialPath(VISOR_CYLINDER_MATERIAL);
	mWarpWidget->AddToViewport(10);
	mIsCylinder2D = true;
	LayoutCylinder2D();
}

void UVisorPrototype::LayoutCylinder2D() const
{
	if (!mWarpWidget || !mIsCylinder2D)
	{
		return;
	}

	const FVector2D viewport = GetViewportSize();
	const float aspect = viewport.Y > 0.0f ? viewport.X / viewport.Y : 16.0f / 9.0f;
	const float tanHalfFov = FMath::Tan(FMath::DegreesToRadians(mCylinderFov * 0.5f));
	mWarpWidget->SetCylinder(tanHalfFov, aspect, FMath::DegreesToRadians(mArcAngle), mGlowStrength, mGlowRadius);
}

void UVisorPrototype::Tune(float arcAngle, float distance)
{
	mArcAngle = FMath::Clamp(arcAngle, 0.0f, 180.0f);
	mDistance = FMath::Max(distance, 11.0f);
	Layout3D();
	LayoutCylinder2D();
}

void UVisorPrototype::SetGlow(float strength, float radius)
{
	mGlowStrength = FMath::Max(strength, 0.0f);
	mGlowRadius = FMath::Max(radius, 0.0f);
	LayoutCylinder2D();
}

void UVisorPrototype::SetWarp(float curvature, float scanlines, float aberration)
{
	mCurvature = curvature;
	mScanlines = scanlines;
	mAberration = aberration;
	if (mWarpWidget && !mIsCylinder2D)
	{
		mWarpWidget->SetWarp(mCurvature, mScanlines, mAberration);
	}
}

void UVisorPrototype::SetHealth(float health)
{
	if (mWidgetComponent)
	{
		if (UVisorHealthWidget* widget = Cast<UVisorHealthWidget>(mWidgetComponent->GetUserWidgetObject()))
		{
			widget->SetHealth(health);
		}
	}
	if (mWarpWidget)
	{
		mWarpWidget->SetHealth(health);
	}
}

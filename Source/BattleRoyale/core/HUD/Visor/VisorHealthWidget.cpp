#include "VisorHealthWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/ProgressBar.h"
#include "Components/ScaleBox.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/FontFace.h"
#include "Fonts/CompositeFont.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Styling/CoreStyle.h"
#include "TimerManager.h"
#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "BattleRoyale/core/Utils/EventDispatcher.h"
#include "BattleRoyale/core/Character/ICharacter.h"

namespace
{
	const FLinearColor TEXT_MAIN = FLinearColor::FromSRGBColor(FColor(0xE6, 0xFB, 0xFF));
	const FLinearColor BADGE_TEXT = FLinearColor::FromSRGBColor(FColor(0x04, 0x18, 0x2C));
	const FVector2D MODULE_SIZE(400.0f, 92.0f);

	// Health states, from the mockups: healthy blue, hurt amber, critical red.
	const FLinearColor HEALTHY = FLinearColor::FromSRGBColor(FColor(30, 150, 255));
	const FLinearColor HURT = FLinearColor::FromSRGBColor(FColor(255, 176, 32));
	const FLinearColor CRITICAL = FLinearColor::FromSRGBColor(FColor(255, 42, 58));
	const FLinearColor CRITICAL_TEXT = FLinearColor::FromSRGBColor(FColor(0xFF, 0x6B, 0x78));
	const FLinearColor HURT_TEXT = FLinearColor::FromSRGBColor(FColor(0xFF, 0xD2, 0x7A));
	constexpr float HURT_START = 0.60f;
	constexpr float HURT_FULL = 0.35f;
	constexpr float CRITICAL_THRESHOLD = 0.25f;
	constexpr float VISUALS_RATE = 1.0f / 30.0f;
	constexpr float DAMAGE_FLASH_DURATION = 0.35f;

	FSlateBrush MakeRoundedBrush(const FLinearColor& fill, const FLinearColor& outline, float outlineWidth, float radius)
	{
		FSlateBrush brush;
		brush.DrawAs = ESlateBrushDrawType::RoundedBox;
		brush.TintColor = FSlateColor(fill);
		brush.OutlineSettings = FSlateBrushOutlineSettings(FVector4(radius, radius, radius, radius), FSlateColor(outline), outlineWidth);
		brush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
		return brush;
	}

	FLinearColor WithAlpha(FLinearColor color, float alpha)
	{
		color.A = alpha;
		return color;
	}

	// Fonts are imported as Font Face assets by Tools/Visor/import_visor_fonts.py; the composite
	// font is built here so no Font asset has to be authored. Falls back to the engine font.
	FSlateFontInfo VisorFont(const TCHAR* faceName, float size)
	{
		static TMap<FString, TSharedPtr<const FCompositeFont>> fonts;
		if (const TSharedPtr<const FCompositeFont>* cached = fonts.Find(faceName))
		{
			return FSlateFontInfo(*cached, size, TEXT("Regular"));
		}

		const FString path = FString::Printf(TEXT("/Game/Fonts/Visor/%s.%s"), faceName, faceName);
		if (const UFontFace* face = LoadObject<UFontFace>(nullptr, *path))
		{
			const TSharedRef<FStandaloneCompositeFont> composite = MakeShared<FStandaloneCompositeFont>();
			FTypefaceEntry entry(TEXT("Regular"));
			entry.Font = FFontData(face);
			composite->DefaultTypeface.Fonts.Add(entry);
			fonts.Add(faceName, composite);
			return FSlateFontInfo(composite, size, TEXT("Regular"));
		}

		UE_LOG(LogTemp, Warning, TEXT("VisorFont: %s not found, run Tools/Visor/import_visor_fonts.py in the editor"), *path);
		return FCoreStyle::GetDefaultFontStyle("Bold", size);
	}
}

TSharedRef<SWidget> UVisorHealthWidget::RebuildWidget()
{
	if (!WidgetTree)
	{
		WidgetTree = NewObject<UWidgetTree>(this, TEXT("WidgetTree"));
	}
	if (!WidgetTree->RootWidget)
	{
		BuildTree();
	}
	return Super::RebuildWidget();
}

void UVisorHealthWidget::BuildTree()
{
	mScaleBox = WidgetTree->ConstructWidget<UScaleBox>(UScaleBox::StaticClass(), TEXT("Supersample"));
	mScaleBox->SetStretch(EStretch::UserSpecified);
	mScaleBox->SetUserSpecifiedScale(mSupersample);
	WidgetTree->RootWidget = mScaleBox;

	mLogicalSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("LogicalSize"));
	mLogicalSizeBox->SetWidthOverride(mLogicalSize.X);
	mLogicalSizeBox->SetHeightOverride(mLogicalSize.Y);
	mScaleBox->AddChild(mLogicalSizeBox);

	UCanvasPanel* root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Root"));
	mLogicalSizeBox->AddChild(root);

	USizeBox* moduleSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("ModuleSize"));
	moduleSize->SetWidthOverride(MODULE_SIZE.X);
	moduleSize->SetHeightOverride(MODULE_SIZE.Y);
	UCanvasPanelSlot* moduleSlot = root->AddChildToCanvas(moduleSize);
	moduleSlot->SetAnchors(FAnchors(0.0f, 1.0f));
	moduleSlot->SetAlignment(FVector2D(0.0f, 1.0f));
	moduleSlot->SetPosition(FVector2D(32.0f, -32.0f));
	moduleSlot->SetAutoSize(true);

	UOverlay* overlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("ModuleOverlay"));
	moduleSize->AddChild(overlay);

	mGlowBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Glow"));
	UOverlaySlot* glowSlot = overlay->AddChildToOverlay(mGlowBorder);
	glowSlot->SetHorizontalAlignment(HAlign_Fill);
	glowSlot->SetVerticalAlignment(VAlign_Fill);
	glowSlot->SetPadding(FMargin(-6.0f));

	mPanelBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Panel"));
	mPanelBorder->SetPadding(FMargin(16.0f, 12.0f));
	UOverlaySlot* panelSlot = overlay->AddChildToOverlay(mPanelBorder);
	panelSlot->SetHorizontalAlignment(HAlign_Fill);
	panelSlot->SetVerticalAlignment(VAlign_Fill);

	UHorizontalBox* row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("Row"));
	mPanelBorder->SetContent(row);

	USizeBox* badgeSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("BadgeSize"));
	badgeSize->SetWidthOverride(36.0f);
	badgeSize->SetHeightOverride(36.0f);
	UHorizontalBoxSlot* badgeSlot = row->AddChildToHorizontalBox(badgeSize);
	badgeSlot->SetVerticalAlignment(VAlign_Center);
	badgeSlot->SetPadding(FMargin(0.0f, 0.0f, 14.0f, 0.0f));

	mBadgeBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Badge"));
	mBadgeBorder->SetHorizontalAlignment(HAlign_Center);
	mBadgeBorder->SetVerticalAlignment(VAlign_Center);
	badgeSize->AddChild(mBadgeBorder);

	UTextBlock* badgeText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("BadgeText"));
	badgeText->SetText(FText::FromString(TEXT("1")));
	badgeText->SetFont(VisorFont(TEXT("ChakraPetch-Bold"), 16));
	badgeText->SetColorAndOpacity(FSlateColor(BADGE_TEXT));
	mBadgeBorder->SetContent(badgeText);

	UVerticalBox* column = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Column"));
	UHorizontalBoxSlot* columnSlot = row->AddChildToHorizontalBox(column);
	columnSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	columnSlot->SetVerticalAlignment(VAlign_Center);

	UHorizontalBox* header = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("Header"));
	UVerticalBoxSlot* headerSlot = column->AddChildToVerticalBox(header);
	headerSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));

	mNameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Name"));
	mNameText->SetFont(VisorFont(TEXT("ChakraPetch-Bold"), 18));
	mNameText->SetColorAndOpacity(FSlateColor(TEXT_MAIN));
	mNameText->SetShadowOffset(FVector2D(0.0f, 1.0f));
	mNameText->SetTextOverflowPolicy(ETextOverflowPolicy::Ellipsis);
	mNameText->SetClipping(EWidgetClipping::ClipToBounds);
	UHorizontalBoxSlot* nameSlot = header->AddChildToHorizontalBox(mNameText);
	nameSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	nameSlot->SetVerticalAlignment(VAlign_Bottom);
	nameSlot->SetPadding(FMargin(0.0f, 0.0f, 10.0f, 0.0f));

	mCriticalTag = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("CriticalTag"));
	mCriticalTag->SetPadding(FMargin(8.0f, 1.0f));
	mCriticalTag->SetBrush(MakeRoundedBrush(CRITICAL, FLinearColor::Transparent, 0.0f, 4.0f));
	mCriticalTag->SetVisibility(ESlateVisibility::Collapsed);
	UHorizontalBoxSlot* tagSlot = header->AddChildToHorizontalBox(mCriticalTag);
	tagSlot->SetVerticalAlignment(VAlign_Center);
	tagSlot->SetPadding(FMargin(0.0f, 0.0f, 10.0f, 0.0f));

	UTextBlock* criticalText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CriticalText"));
	criticalText->SetText(FText::FromString(TEXT("CRÍTICA")));
	criticalText->SetFont(VisorFont(TEXT("BarlowSemiCondensed-SemiBold"), 12));
	criticalText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	mCriticalTag->SetContent(criticalText);

	mValueText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Value"));
	mValueText->SetFont(VisorFont(TEXT("ChakraPetch-Bold"), 26));
	mValueText->SetShadowOffset(FVector2D(0.0f, 1.0f));
	UHorizontalBoxSlot* valueSlot = header->AddChildToHorizontalBox(mValueText);
	valueSlot->SetVerticalAlignment(VAlign_Bottom);

	USizeBox* barSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("BarSize"));
	barSize->SetHeightOverride(16.0f);
	column->AddChildToVerticalBox(barSize);

	mHealthBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("HealthBar"));
	barSize->AddChild(mHealthBar);

	FProgressBarStyle barStyle;
	barStyle.BackgroundImage = MakeRoundedBrush(FLinearColor(0.0f, 0.0f, 0.0f, 0.5f), FLinearColor::Transparent, 0.0f, 8.0f);
	barStyle.FillImage = MakeRoundedBrush(FLinearColor::White, FLinearColor::Transparent, 0.0f, 8.0f);
	barStyle.MarqueeImage = barStyle.FillImage;
	mHealthBar->SetWidgetStyle(barStyle);

	SetHealth(mHealth);
}

void UVisorHealthWidget::ApplyStyle()
{
	if (!mPanelBorder)
	{
		return;
	}

	const float percent = MaxHealth > 0.0f ? FMath::Clamp(mHealth / MaxHealth, 0.0f, 1.0f) : 0.0f;
	const float hurt = FMath::Clamp((HURT_START - percent) / (HURT_START - HURT_FULL), 0.0f, 1.0f);
	const float critical = FMath::Clamp((HURT_FULL - percent) / (HURT_FULL - CRITICAL_THRESHOLD), 0.0f, 1.0f);
	const bool isCritical = percent <= CRITICAL_THRESHOLD;
	// Critical: the whole panel breathes about twice per second.
	const float pulse = isCritical ? 0.5f + 0.5f * FMath::Sin(mVisualsTime * 2.0f * PI * 1.8f) : 0.0f;

	const FLinearColor stateColor = FMath::Lerp(FMath::Lerp(mAccent, HURT, hurt), CRITICAL, critical);
	const FLinearColor fillColor = FMath::Lerp(FMath::Lerp(HEALTHY, HURT, hurt), CRITICAL, critical);
	float fillAlpha = FMath::Lerp(FMath::Lerp(0.30f, 0.34f, hurt), 0.52f, critical) + pulse * 0.22f;
	FLinearColor fill = WithAlpha(fillColor, fillAlpha);
	if (mDamageFlash > 0.0f)
	{
		fill = FMath::Lerp(fill, FLinearColor(1.0f, 1.0f, 1.0f, 0.75f), mDamageFlash * 0.55f);
	}

	mGlowBorder->SetBrush(MakeRoundedBrush(FLinearColor::Transparent, WithAlpha(stateColor, 0.18f + pulse * 0.35f), 6.0f + pulse * 4.0f, 24.0f));
	mPanelBorder->SetBrush(MakeRoundedBrush(fill, stateColor, 2.0f + critical, 18.0f));
	mBadgeBorder->SetBrush(MakeRoundedBrush(stateColor, WithAlpha(stateColor, 0.6f), 2.0f, 18.0f));

	const FLinearColor textColor = isCritical ? FLinearColor::White : FMath::Lerp(TEXT_MAIN, HURT_TEXT, hurt);
	const FLinearColor barColor = FMath::Lerp(FMath::Lerp(TEXT_MAIN, HURT, hurt), CRITICAL_TEXT, critical);
	mValueText->SetColorAndOpacity(FSlateColor(textColor));
	mHealthBar->SetFillColorAndOpacity(barColor);
	mCriticalTag->SetVisibility(isCritical ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
}

void UVisorHealthWidget::SetSupersample(float scale, const FVector2D& logicalSize)
{
	mSupersample = FMath::Max(scale, 0.1f);
	mLogicalSize = logicalSize;
	if (mScaleBox)
	{
		mScaleBox->SetUserSpecifiedScale(mSupersample);
		mLogicalSizeBox->SetWidthOverride(mLogicalSize.X);
		mLogicalSizeBox->SetHeightOverride(mLogicalSize.Y);
	}
}

void UVisorHealthWidget::SetAccentColor(const FLinearColor& accent)
{
	mAccent = accent;
	ApplyStyle();
}

void UVisorHealthWidget::SetHealth(float health)
{
	if (health < mHealth)
	{
		mDamageFlash = 1.0f;
	}
	mHealth = health;
	if (!mValueText)
	{
		return;
	}

	const float percent = MaxHealth > 0.0f ? FMath::Clamp(health / MaxHealth, 0.0f, 1.0f) : 0.0f;
	mValueText->SetText(FText::AsNumber(FMath::RoundToInt(health)));
	mHealthBar->SetPercent(percent);
	ApplyStyle();
}

void UVisorHealthWidget::TickVisuals()
{
	mVisualsTime += VISUALS_RATE;
	const bool isCritical = MaxHealth > 0.0f && mHealth / MaxHealth <= CRITICAL_THRESHOLD;
	if (mDamageFlash > 0.0f || isCritical)
	{
		mDamageFlash = FMath::Max(0.0f, mDamageFlash - VISUALS_RATE / DAMAGE_FLASH_DURATION);
		ApplyStyle();
	}
}

void UVisorHealthWidget::NativeConstruct()
{
	Super::NativeConstruct();

	const APlayerController* playerController = GetOwningPlayer();
	const APlayerState* playerState = playerController ? playerController->PlayerState : nullptr;
	mNameText->SetText(FText::FromString(playerState ? playerState->GetPlayerName() : TEXT("Jugador")));

	// Health is only broadcast on change, so start from the pawn's current value (without a flash).
	if (const IICharacter* character = Cast<IICharacter>(playerController ? playerController->GetPawn() : nullptr))
	{
		SetHealth(character->GetCurrentHealth());
		mDamageFlash = 0.0f;
	}

	if (const auto gameInstance = Cast<UBattleRoyaleGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		gameInstance->GetEventDispatcher()->OnRefreshHealth.AddUniqueDynamic(this, &ThisClass::OnRefreshHealth);
	}

	if (const UWorld* world = GetWorld())
	{
		world->GetTimerManager().SetTimer(mVisualsTimer, FTimerDelegate::CreateUObject(this, &ThisClass::TickVisuals), VISUALS_RATE, true);
	}
}

void UVisorHealthWidget::NativeDestruct()
{
	if (const auto gameInstance = Cast<UBattleRoyaleGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		gameInstance->GetEventDispatcher()->OnRefreshHealth.RemoveDynamic(this, &ThisClass::OnRefreshHealth);
	}
	if (const UWorld* world = GetWorld())
	{
		world->GetTimerManager().ClearTimer(mVisualsTimer);
	}

	Super::NativeDestruct();
}

void UVisorHealthWidget::OnRefreshHealth(float health)
{
	SetHealth(health);
}

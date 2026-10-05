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
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Styling/CoreStyle.h"
#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "BattleRoyale/core/Utils/EventDispatcher.h"
#include "BattleRoyale/core/Character/ICharacter.h"

namespace
{
	const FLinearColor TEXT_MAIN = FLinearColor::FromSRGBColor(FColor(0xE6, 0xFB, 0xFF));
	const FLinearColor BADGE_TEXT = FLinearColor::FromSRGBColor(FColor(0x04, 0x18, 0x2C));
	const FLinearColor CRITICAL = FLinearColor::FromSRGBColor(FColor(0xFF, 0x6B, 0x78));
	const FVector2D MODULE_SIZE(400.0f, 92.0f);

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

	// A wider, faint outline behind the panel stands in for the glow until the final technique is chosen.
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
	badgeText->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 16));
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
	mNameText->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 18));
	mNameText->SetColorAndOpacity(FSlateColor(TEXT_MAIN));
	mNameText->SetShadowOffset(FVector2D(0.0f, 1.0f));
	mNameText->SetTextOverflowPolicy(ETextOverflowPolicy::Ellipsis);
	mNameText->SetClipping(EWidgetClipping::ClipToBounds);
	UHorizontalBoxSlot* nameSlot = header->AddChildToHorizontalBox(mNameText);
	nameSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	nameSlot->SetVerticalAlignment(VAlign_Bottom);
	nameSlot->SetPadding(FMargin(0.0f, 0.0f, 12.0f, 0.0f));

	mValueText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Value"));
	mValueText->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 26));
	mValueText->SetShadowOffset(FVector2D(0.0f, 1.0f));
	UHorizontalBoxSlot* valueSlot = header->AddChildToHorizontalBox(mValueText);
	valueSlot->SetVerticalAlignment(VAlign_Bottom);

	USizeBox* barSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("BarSize"));
	barSize->SetHeightOverride(16.0f);
	column->AddChildToVerticalBox(barSize);

	mHealthBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("HealthBar"));
	barSize->AddChild(mHealthBar);

	ApplyStyle();
	SetHealth(mHealth);
}

void UVisorHealthWidget::ApplyStyle()
{
	if (!mPanelBorder)
	{
		return;
	}

	mGlowBorder->SetBrush(MakeRoundedBrush(FLinearColor::Transparent, WithAlpha(mAccent, 0.25f), 6.0f, 24.0f));
	mPanelBorder->SetBrush(MakeRoundedBrush(FLinearColor(0.012f, 0.18f, 0.6f, 0.30f), mAccent, 2.0f, 18.0f));
	mBadgeBorder->SetBrush(MakeRoundedBrush(mAccent, WithAlpha(mAccent, 0.6f), 2.0f, 18.0f));

	FProgressBarStyle barStyle;
	barStyle.BackgroundImage = MakeRoundedBrush(FLinearColor(0.0f, 0.0f, 0.0f, 0.5f), FLinearColor::Transparent, 0.0f, 8.0f);
	barStyle.FillImage = MakeRoundedBrush(FLinearColor::White, FLinearColor::Transparent, 0.0f, 8.0f);
	barStyle.MarqueeImage = barStyle.FillImage;
	mHealthBar->SetWidgetStyle(barStyle);
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
	mHealth = health;
	if (!mValueText)
	{
		return;
	}

	const float percent = MaxHealth > 0.0f ? FMath::Clamp(health / MaxHealth, 0.0f, 1.0f) : 0.0f;
	const FLinearColor healthColor = percent <= 0.25f ? CRITICAL : TEXT_MAIN;

	mValueText->SetText(FText::AsNumber(FMath::RoundToInt(health)));
	mValueText->SetColorAndOpacity(FSlateColor(healthColor));
	mHealthBar->SetPercent(percent);
	mHealthBar->SetFillColorAndOpacity(healthColor);
}

void UVisorHealthWidget::NativeConstruct()
{
	Super::NativeConstruct();

	const APlayerController* playerController = GetOwningPlayer();
	const APlayerState* playerState = playerController ? playerController->PlayerState : nullptr;
	mNameText->SetText(FText::FromString(playerState ? playerState->GetPlayerName() : TEXT("Jugador")));

	// Health is only broadcast on change, so start from the pawn's current value.
	if (const IICharacter* character = Cast<IICharacter>(playerController ? playerController->GetPawn() : nullptr))
	{
		SetHealth(character->GetCurrentHealth());
	}

	if (const auto gameInstance = Cast<UBattleRoyaleGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		gameInstance->GetEventDispatcher()->OnRefreshHealth.AddUniqueDynamic(this, &ThisClass::OnRefreshHealth);
	}
}

void UVisorHealthWidget::NativeDestruct()
{
	if (const auto gameInstance = Cast<UBattleRoyaleGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		gameInstance->GetEventDispatcher()->OnRefreshHealth.RemoveDynamic(this, &ThisClass::OnRefreshHealth);
	}

	Super::NativeDestruct();
}

void UVisorHealthWidget::OnRefreshHealth(float health)
{
	SetHealth(health);
}

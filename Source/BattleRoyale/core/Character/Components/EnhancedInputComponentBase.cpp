
#include "EnhancedInputComponentBase.h"

#include "EnhancedInputSubsystems.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/Utils/Input/InputMappingContextWithBindings.h"

UEnhancedInputComponentBase::UEnhancedInputComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEnhancedInputComponentBase::Initialize(UInputMappingContextWithBindings* defaultMappingContext, const APlayerController* playerController)
{
	if(!playerController)
	{
		return;
	}

	if(!defaultMappingContext)
	{
		UE_LOG(LogCharacter, Log, TEXT("UEnhancedInputComponentBase::Initialize DefaultMappingContext is not assigned"));
		return;	
	}
	
	const auto enhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer());
	if(!enhancedInputSubsystem)
	{
		return;
	}
	mDefaultMappingContext = defaultMappingContext;
	enhancedInputSubsystem->ClearAllMappings();
	enhancedInputSubsystem->AddMappingContext(defaultMappingContext->GetContext(), 0);
}

void UEnhancedInputComponentBase::BindInputs(UObject* owner)
{
	mDefaultMappingContext->BindInputs(this, owner);
}



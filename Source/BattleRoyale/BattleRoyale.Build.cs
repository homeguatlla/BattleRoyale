// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BattleRoyale : ModuleRules
{
	public BattleRoyale(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"HeadMountedDisplay",
			//online subsystem
			"OnlineSubsystem",
			"OnlineSubsystemUtils",
			//GAS
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			//HUD
			"UMG"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			//Automation tests (in order to include AutomationEditorCommon.h)
			//"UnrealEd"
		});

		DynamicallyLoadedModuleNames.AddRange(new string[]
		{
			"OnlineSubsystemNull"
		});
	}
}
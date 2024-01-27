using UnrealBuildTool;

public class BattleRoyaleTests : ModuleRules
{
    public BattleRoyaleTests(ReadOnlyTargetRules Target) : base(Target)
    {
                PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
                bUseRTTI = true;
                
                PrivateDependencyModuleNames.AddRange(new string[] {
                    "Core",
                    "Engine",
                    "CoreUObject",
                    "BattleRoyale",
                    //Automation tests (in order to include AutomationEditorCommon.h)
                    "UnrealEd",
                    //GAS
                    "GameplayTags"
                });
    }
}
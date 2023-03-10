using UnrealBuildTool;

public class BattleRoyaleTests : ModuleRules
{
    public BattleRoyaleTests(ReadOnlyTargetRules Target) : base(Target)
    {
                PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
                PrivateDependencyModuleNames.AddRange(new string[] {
                    "Core",
                    "Engine",
                    "CoreUObject",
                    "BattleRoyale"
                });
    }
}
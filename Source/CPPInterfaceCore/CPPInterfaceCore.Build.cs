// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CPPInterfaceCore : ModuleRules
{
	public CPPInterfaceCore(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.CPlusPlus;

        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[]
        {
	        "Core",
        });
        
        if (Target.Configuration != UnrealTargetConfiguration.Shipping)
        {
	        PublicDependencyModuleNames.Add("CoreUObject");
    
	        PrivateDependencyModuleNames.AddRange(new string[]
	        {
		        "AutomationController",
	        });
        }
        else
        {
	        PrivateDependencyModuleNames.Clear();
        }
        
        bEnableExceptions = true;
        CppStandard = CppStandardVersion.Cpp20; 
        
        PublicIncludePaths.Add(ModuleDirectory + "/Public");
        PrivateIncludePaths.Add(ModuleDirectory + "/Private");

    }
}

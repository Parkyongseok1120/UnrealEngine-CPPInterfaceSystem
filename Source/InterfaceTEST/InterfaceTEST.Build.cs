// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;


public class InterfaceTEST : ModuleRules
{
	public InterfaceTEST(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicIncludePaths.Add(ModuleDirectory);
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput", 
			"CPPInterfaceCore"
		});

		PublicIncludePaths.AddRange(new string[] { });

		PublicIncludePaths.Add(ModuleDirectory + "/../CPPInterfaceCore/Public");
		
		if (Target.Configuration != UnrealTargetConfiguration.Shipping)
		{
			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"AutomationController",
				"FunctionalTesting",
			});
		}

		PrivateDependencyModuleNames.Add("TraceLog");

    }
}

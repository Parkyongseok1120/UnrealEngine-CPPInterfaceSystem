// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;


public class InterfaceTEST : ModuleRules
{
	public InterfaceTEST(ReadOnlyTargetRules Target) : base(Target)
	{
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



    }
}

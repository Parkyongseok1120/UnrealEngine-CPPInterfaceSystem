// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CPPInterfaceCore : ModuleRules
{
	public CPPInterfaceCore(ReadOnlyTargetRules Target) : base(Target)
	{
        Type = ModuleType.External;

        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // UE 의존성 완전제거
        PublicDependencyModuleNames.Clear();
        PrivateDependencyModuleNames.Clear();

        // 표준 C++ 라이브러리만 사용
        bEnableExceptions = true;
        CppStandard = CppStandardVersion.Cpp17; 

        // Include 경로
        PublicIncludePaths.Add(ModuleDirectory + "/Public");
        PrivateIncludePaths.Add(ModuleDirectory + "/Private");

    }
}

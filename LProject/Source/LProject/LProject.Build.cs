// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LProject : ModuleRules
{
	public LProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[]
		{
			"LProject",
        });

        PublicDependencyModuleNames.AddRange(new string[] 
		{
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "SlateCore",
            "EnhancedInput",
            "GameplayTags",
            "NavigationSystem",
            "AIModule",
            "Niagara",
            "UMG",
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });

    }
}

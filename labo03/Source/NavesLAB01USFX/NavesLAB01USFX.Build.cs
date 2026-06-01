// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NavesLAB01USFX : ModuleRules
{
	public NavesLAB01USFX(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
	}
}

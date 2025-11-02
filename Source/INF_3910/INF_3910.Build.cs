// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class INF_3910 : ModuleRules
{
	public INF_3910(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput", 
			"GameplayAbilities", 
			"GameplayTasks", 
			"GameplayTags",
			"NetCore", 
			"UMG",
			"HTTP",
			"Json",
			"JsonUtilities",
			"AIModule"
		});
		
		PrivateDependencyModuleNames.AddRange(new string[] { "ProceduralMeshComponent" });
	}
}

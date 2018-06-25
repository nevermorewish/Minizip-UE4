// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
public class MiniZip : ModuleRules
{
	public MiniZip(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"MiniZip/Public"
				// ... add public include paths required here ...
			}
			);

        Definitions.Add("HAVE_ZLIB");
        PrivateIncludePaths.AddRange(
			new string[] {
				"MiniZip/Private",
                Path.GetFullPath(Path.Combine(ModuleDirectory, "MiniZip")),
				 Path.GetFullPath(Path.Combine(ModuleDirectory, "Zipper")),
        // ... add other private include paths required here ...
    }
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "zlib",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}

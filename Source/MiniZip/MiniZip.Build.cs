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

        PrivateIncludePaths.AddRange(
			new string[] {
				"MiniZip/Private",
                Path.GetFullPath(Path.Combine(ModuleDirectory, "MiniZip")),
        // ... add other private include paths required here ...
				}
			);

        if (Target.Platform != UnrealTargetPlatform.Win64)
        {
            Definitions.Add("IOAPI_NO_64");
        }
        
        

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "zlib",
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

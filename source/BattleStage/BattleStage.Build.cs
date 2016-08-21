// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BattleStage : ModuleRules
{
	public BattleStage(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "OnlineSubsystem",
                "OnlineSubsystemUtils",
                "UMG",
            });
    
        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "HeadMountedDisplay",
                "InputCore",
                "Slate",
                "SlateCore",                
            });

        DynamicallyLoadedModuleNames.AddRange(
            new string[] {
                "OnlineSubsystemNull"
            });
    }
}

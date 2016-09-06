// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BattleStage : ModuleRules
{
	public BattleStage(TargetInfo Target)
	{

        PrivateIncludePaths.AddRange(
            new string[] {
                "BattleStage/Private",
                "BattleStage/Private/UI",
                "BattleStage/Private/UI/UMG",
                "BattleStage/Private/UI/UMG/HUD",
                "BattleStage/Private/Effects",
                "BattleStage/Private/GameModes",
                "BattleStage/Private/Online",
                "BattleStage/Private/Player",
                "BattleStage/Private/Weapons",
                "BattleStage/Private/Sound",
            });

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

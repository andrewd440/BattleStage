// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BattleStage : ModuleRules
{
	public BattleStage(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

        /* VR Module */
        PrivateDependencyModuleNames.AddRange(new string[] { "HeadMountedDisplay" });
    }
}

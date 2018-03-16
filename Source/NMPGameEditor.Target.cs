// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class NMPGameEditorTarget : TargetRules
{
	public NMPGameEditorTarget(TargetInfo Target) : base(Target)
    {
		Type = TargetType.Editor;

        ExtraModuleNames.AddRange(new string[] { "NMPGame" });
    }
}

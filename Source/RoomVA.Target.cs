using UnrealBuildTool;
using System.Collections.Generic;

public class RoomVATarget : TargetRules
{
	public RoomVATarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("RoomVA");
	}
}

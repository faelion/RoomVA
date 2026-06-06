using UnrealBuildTool;
using System.Collections.Generic;

public class RoomVAEditorTarget : TargetRules
{
	public RoomVAEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("RoomVA");
	}
}

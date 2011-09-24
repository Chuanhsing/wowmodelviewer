#include "Settings_Exporters.h"

void ExporterSettingInit()
{
	// Groups will be used for each Export's specific variables. They are done with a Prefix, such as "GroupName/VarName".
	// Settings without a prefix will be listed in the General Group.
	SettingsExList.insert("Version",MajorBuildVersion);
	SettingsExList.insert("Export_Perferred",-1);
	SettingsExList.insert("Export_InitOnly",true);
	SettingsExList.insert("Export_UseWMVPosRot",false);
	SettingsExList.insert("Export_ScaleToRealWorld",false);
}
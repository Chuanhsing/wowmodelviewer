#ifndef MAINSETTINGS_H
#define MAINSETTINGS_H

#include <QSettings>
#include <QDir>
#include <QMap>
#include "version.h"

#define WMV_INI_NAME "wmv.ini"
#define WOWDIRS_INI_NAME "wowdirs.ini"

// Settings
/* Registry version disabled until we can get the settings to save and load correctly.
#ifndef _DEBUG
	//Use the Registry settings for non-debug versions.
	QSettings sWMVSettings("WMV Community","WoW Model Viewer");		// Registry Version Settings
#else
	// Use an INI file for debugging settings and values.
	*/
	static QSettings sWMVSettings(QDir::currentPath()+"/"+WMV_INI_NAME,QSettings::IniFormat);	// Ini Version Settings
//#endif
static QSettings sWoWDirs(QDir::currentPath()+"/"+WOWDIRS_INI_NAME,QSettings::IniFormat);	// WoW Directory List

static QMap<QString,QVariant> SettingsList;

// Initalization of our default settings
static void SettingsListInit()
{
	// Groups are done with a Prefix, such as "GroupName/VarName". Settings without a prefix will be listed in the General Group.
	SettingsList.insert("Version",MAJORVERSION+BUILDVERSION);		// Used to compare the settings version against the program's version.
}

// Check to see if the Main settings exist, and if not, set them.
void CheckSettings_Main();

#endif
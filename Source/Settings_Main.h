#ifndef MAINSETTINGS_H
#define MAINSETTINGS_H

#include <QSettings>
#include <QDir>
#include <QMap>
#include "version.h"
#include "classes.h"

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
static QMap<QString,st_WoWDir> WoWDirList;

// Initalization of our default settings
static void SettingsListInit()
{
	// Groups are done with a Prefix, such as "GroupName/VarName". Settings without a prefix will be listed in the General Group.
	SettingsList.insert("Version",MajorBuildVersion);		// Used to compare the settings version against the program's version.
	SettingsList.insert("LastInterfaceMode",INTERFACEMODE_VIEWER);	// Default Interface mode.
	SettingsList.insert("CurrentWoWDir","None");					// Sets the Current WoW Dirto a String. String will match the names in the WoWDirList.
}

// Check to see if the Main settings exist, and if not, set them.
void CheckSettings_Main();

// Process the sWoWDirs and dump them into the WoWDirList
void ReadWoWDirList();

#endif
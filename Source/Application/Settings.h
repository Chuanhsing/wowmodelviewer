#ifndef WMV_SETTINGS_APPLICATION_H
#define WMV_SETTINGS_APPLICATION_H

#include "../Engine/wmv_engine.h"
#include <QSettings>
#include <QDir>
#include <QMap>
#include "../Engine/version.h"
#include "../Engine/classes.h"

namespace WMVEngine {

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

extern QMap<QString,QVariant> SettingsList;
extern QMap<QString,st_WoWDir> WoWDirList;

// Initalization of our default settings
void SettingsListInit();

// Check to see if the Main settings exist, and if not, set them.
void CheckSettings_Main();

// Process the sWoWDirs and dump them into the WoWDirList
void ReadWoWDirList();

} // Namespace

#endif
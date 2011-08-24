#ifndef EXPORTER_SETTINGS_H
#define EXPORTER_SETTINGS_H

#include <QSettings>
#include <QDir>
#include <QMap>
#include <QString>
#include <QVariant>
#include "../version.h"

#define EXPORTER_INI_NAME "exporters.ini"

// Exporter Settings
static QSettings sWMVExSettings(QDir::currentPath()+"/"+EXPORTER_INI_NAME,QSettings::IniFormat);
static QMap<QString, QVariant> SettingsExList;		// This can be accessed by the Exporters, so they can include their own settings.

// Set the Exporter Defaults
void ExporterSettingInit()
{
	// Groups will be used for each Export's specific variables. They are done with a Prefix, such as "GroupName/VarName".
	// Settings without a prefix will be listed in the General Group.
	SettingsExList.insert("Version",MAJORVERSION+BUILDVERSION);
	SettingsExList.insert("Export_Perferred",-1);
	SettingsExList.insert("Export_InitOnly",true);
	SettingsExList.insert("Export_UseWMVPosRot",false);
	SettingsExList.insert("Export_ScaleToRealWorld",false);
};

#endif
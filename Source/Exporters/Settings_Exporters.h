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
void ExporterSettingInit();

#endif
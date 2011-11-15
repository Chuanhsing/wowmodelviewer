/*
	This file hosts our version numbers, and any other "static" variables that we might need to change between builds.
*/
#ifndef WMV_VERSION_H
#define WMV_VERSION_H

namespace WMVEngine{

#include <QObject>
#include <QString>
#include "constants.h"

#define PROGRAMNAME static_cast<const char*>(QObject::tr("World of Warcraft Model Viewer","WMVProgramName").toUtf8())
#define MAJORVERSION "v0.8.0.0"
#define BUILDVERSION "r642"
#define ORGANIZATIONNAME static_cast<const char*>(QObject::tr("WoW Model Viewer Community","WMVCommunityName").toUtf8())
#define ORGANIZATIONWEBSITE "http://www.wowmodelviewer.org/forum/index.php"
#define SOURCEWEBSITE "http://code.google.com/p/wowmodelviewer/"

const QString MajorBuildVersion(QString(MAJORVERSION) + QString(BUILDVERSION));

}

#endif
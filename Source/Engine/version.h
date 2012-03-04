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
#define BUILDVERSIONNAME "Tofu"
#define ORGANIZATIONNAME static_cast<const char*>(QObject::tr("WoW Model Viewer Community","WMVCommunityName").toUtf8())
#define ORGANIZATIONWEBSITE "http://www.wowmodelviewer.org/"
#define SOURCEWEBSITE "http://wowmodelviewer.googlecode.com/"

const QString MajorBuildVersion(QString(MAJORVERSION));

}

#endif
/*
	This file hosts our version numbers, and any other "static" variables that we might need to change between builds.
*/
#ifndef WMV_VERSION_H
#define WMV_VERSION_H

#include <QObject>
#include <QString>
#include "constants.h"

#define PROGRAMNAME QObject::tr("World of Warcraft Model Viewer","WMVProgramName")
#define MAJORVERSION QString("v0.8.0.0")
#define BUILDVERSION QString("r625")
#define ORGANIZATIONNAME QObject::tr("WoW Model Viewer Community","WMVCommunityName")
#define ORGANIZATIONWEBSITE QString("http://www.wowmodelviewer.org/forum/index.php")
#define SOURCEWEBSITE QString("http://code.google.com/p/wowmodelviewer/")

#endif
/*
	This file hosts our version numbers, and any other "static" variables that we might need to change between builds.
*/
#ifndef WMV_VERSION_H
#define WMV_VERSION_H

// Logging Includes
#ifdef _DEBUG
	// Debug mode gives us file names and line numbers in the log file
	#define QS_LOG_LINE_NUMBERS 1
#endif
#include "QsLog.h"
#include "QsLogDest.h"

#include <QObject>
#include <QString>
#include "constants.h"

#define PROGRAMNAME static_cast<const char*>(QObject::tr("World of Warcraft Model Viewer","WMVProgramName").toUtf8())
#define MAJORVERSION "v0.8.0.0"
#define BUILDVERSION "r626"
#define ORGANIZATIONNAME static_cast<const char*>(QObject::tr("WoW Model Viewer Community","WMVCommunityName").toUtf8())
#define ORGANIZATIONWEBSITE "http://www.wowmodelviewer.org/forum/index.php"
#define SOURCEWEBSITE "http://code.google.com/p/wowmodelviewer/"

const QString MajorBuildVersion(QString(MAJORVERSION) + QString(BUILDVERSION));

#endif
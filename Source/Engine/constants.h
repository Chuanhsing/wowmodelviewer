/*
	Never-Changing Variables, and more, such as our OS-Dependent global variables.
*/
#ifndef WMV_CONSTANTS_H
#define WMV_CONSTANTS_H

#include <QObject>
#include "../Foundation/Platforms/Compilers.h"

namespace WMVEngine {

// Filenames & Paths
#define LOGFILE_FILENAME			"log.txt"
#define PLUGIN_PATH					"./Plugins/"	// Relative to the application

}	// Namespace

#if defined(_DEBUG) || defined (DEBUG)
	#define DEBUGVERSION static_cast<const char*>(QObject::tr(" Debug").toUtf8())
#else
	#define DEBUGVERSION ""
#endif

// Various Defines
#ifndef HANDLE
	typedef void* HANDLE;
#endif

#endif
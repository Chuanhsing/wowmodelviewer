/*
	Never-Changing Variables, and more, such as our OS-Dependent global variables.
*/
#ifndef WMV_CONSTANTS_H
#define WMV_CONSTANTS_H

#include <QObject>
#include <QMap>
#include <QString>

// Log.txt Filename
#define LOGTXTFILENAME "log.txt"

// Directory Slash type for MPQ files
#define SLASH_MPQ "\\"

#if defined (_WINDOWS)
	#if defined (_WIN64)
		#define SYSTEMVERSION static_cast<const char*>(QObject::tr("Windows 64-bit").toUtf8())
	#elif defined (_WIN32)
		#define SYSTEMVERSION static_cast<const char*>(QObject::tr("Windows 32-bit").toUtf8())
	#else
		#error "Your Windows platform is not defined. Please specify either _WIN64 or _WIN32."
	#endif
#elif defined (_MAC)
	#if  defined (_MAC64)
		#define SYSTEMVERSION static_cast<const char*>(QObject::tr("Macintosh 64-bit").toUtf8())
	#elif defined (_MAC32)
		#define SYSTEMVERSION static_cast<const char*>(QObject::tr("Macintosh 32-bit").toUtf8())
	#else
		#error "Your Macintosh platform is not defined. Please specify either _MAC64 or _MAC32."
	#endif
#elif defined (_LINUX)
	#if defined (_LINUX64)
		#define SYSTEMVERSION static_cast<const char*>(QObject::tr("Linux 64-bit").toUtf8())
	#elif defined (_LINUX32)
		#define SYSTEMVERSION static_cast<const char*>(QObject::tr("Linux 32-bit").toUtf8())
	#else
		#error "Your Linux platform is not defined. Please specify either _LINUX64 or _LINUX32."
	#endif
#else
	#ifdef _DEBUG
		#error "You have not specified a valid Operating System for your Debug configuration."
	#else
		#error "You have not specified a valid Operating System for your Release configuration."
	#endif
#endif

#ifdef _DEBUG
	#define DEBUGVERSION static_cast<const char*>(QObject::tr(" Debug").toUtf8())
#else
	#define DEBUGVERSION ""
#endif

// Various Defines

#ifndef HANDLE
typedef void* HANDLE;
#endif
/*
#ifndef ssize_t
	#if defined (_WIN64) // || defined (_LINUX64)	// Not sure about this...
		typedef __int64 ssize_t;
	#else
		typedef __int32 ssize_t;
	#endif
#endif
*/
#endif

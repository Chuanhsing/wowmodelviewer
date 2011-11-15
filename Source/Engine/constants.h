/*
	Never-Changing Variables, and more, such as our OS-Dependent global variables.
*/
#ifndef WMV_CONSTANTS_H
#define WMV_CONSTANTS_H

#include <QObject>

// Filenames & Paths
#define LOGFILE_FILENAME			"log.txt"
#define PLUGIN_PATH					"./Plugins/"	// Relative to the application

// Directory Slash type for MPQ files
#define SLASH_MPQ "\\"

// Operating System dependant variables.
#if defined (_WINDOWS)
	// Operating System 32/64 Bits
	#if defined (_WIN64)
		#define SYSTEMVERSION static_cast<const char*>(QObject::tr("Windows 64-bit").toUtf8())
	#elif defined (_WIN32)
		#define SYSTEMVERSION static_cast<const char*>(QObject::tr("Windows 32-bit").toUtf8())
	#else
		#error "Your Windows platform is not defined. Please specify either _WIN64 or _WIN32."
	#endif
#elif defined (_MAC)
	// Operating System 32/64 Bits
	#if  defined (_MAC64)
		#define SYSTEMVERSION static_cast<const char*>(QObject::tr("Macintosh 64-bit").toUtf8())
	#elif defined (_MAC32)
		#define SYSTEMVERSION static_cast<const char*>(QObject::tr("Macintosh 32-bit").toUtf8())
	#else
		#error "Your Macintosh platform is not defined. Please specify either _MAC64 or _MAC32."
	#endif
#elif defined (_LINUX)
	// Operating System 32/64 Bits
	#if defined (_LINUX64)
		#define SYSTEMVERSION static_cast<const char*>(QObject::tr("Linux 64-bit").toUtf8())
	#elif defined (_LINUX32)
		#define SYSTEMVERSION static_cast<const char*>(QObject::tr("Linux 32-bit").toUtf8())
	#else
		#error "Your Linux platform is not defined. Please specify either _LINUX64 or _LINUX32."
	#endif
	// Desktop type, with Text Viewer defined.
	#ifdef _LINUX_GNOME
		#define LINUX_TXT_VIEWER "gedit"
	#elif defined (_LINUX_KDE)
		#define LINUX_TXT_VIEWER "kedit"
	#else
		#error "Your Linux GUI is not defined. Please specify _LINUX_GNOME, _LINUX_KDE, or add a new one, specifying it's text viewing program."
	#endif
	#ifndef LINUX_TXT_VIEWER
		#error "You do not have LINUX_TXT_VIEWER defined with your text viewing program. Either define your GUI type, or add a new definition to include your GUI type, and it's text viewer/editor."
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

// Needed by Visual Studio, since ssize_t isn't natively defined
#if defined (_MSC_VER)
	#if !defined (ssize_t)
		#if defined (_WIN64)
			typedef __int64 ssize_t;
		#else
			typedef __int32 ssize_t;
		#endif
	#endif
#endif

#endif
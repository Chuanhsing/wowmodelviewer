#ifndef PLATFORM_DEFINE_H
#define PLATFORM_DEFINE_H

// Clear any defines from StormPort.h
#undef PLATFORM_WINDOWS
#undef PLATFORM_MAC
#undef PLATFORM_LINUX

// Define Basic Platforms
#define PLATFORM_WINDOWS	0
#define PLATFORM_MAC		1
#define PLATFORM_LINUX		2

// Determine our Platform
#if defined (_WINDOWS) || defined(_WIN64) || defined (_WIN32) || defined (WIN32) || defined (__WIN32__)
#	define PLATFORM PLATFORM_WINDOWS
#elif defined (__APPLE__) || defined (__APPLE_CC__) || defined (_MAC) || defined (_MAC64) || defined (_MAC32) || defined (__powerpc) || defined (__ppc__) || defined (__powerpc64__)
#	define PLATFORM PLATFORM_MAC
#elif defined (__linux) || defined (linux) || defined (__linux__)
#	define PLATFORM PLATFORM_LINUX
#endif

#ifndef PLATFORM
#	ifdef _DEBUG
#		error "You have not specified a valid Operating System for your Debug configuration."
#	else
#		error "You have not specified a valid Operating System for your Release configuration."
#	endif
#endif

// Set Platform-specific options
#if PLATFORM == PLATFORM_WINDOWS
#	if defined (_WIN64)
#		define SYSTEMVERSION static_cast<const char*>(QObject::tr("Windows 64-bit").toUtf8())
#	elif defined (_WIN32 ) || defined (WIN32) || defined (__WIN32__)
#		define SYSTEMVERSION static_cast<const char*>(QObject::tr("Windows 32-bit").toUtf8())
#	else
#		error "Your Windows platform is not properly defined. Could not determine architecture."
#	endif
#endif
#if PLATFORM == PLATFORM_MAC
#	if defined (__powerpc) || defined (__ppc__) || defined (__powerpc64__)
#		error "This program does not support the Power PC architecture."
#	elif defined (__x86_64__) || defined(__LP64__) || defined(_LP64) || defined (_MAC64)
#		define SYSTEMVERSION static_cast<const char*>(QObject::tr("Macintosh 64-bit").toUtf8())
#	elif defined (__i386__) || defined (__i386) || defined (i386) || defined (_MAC32)
#		define SYSTEMVERSION static_cast<const char*>(QObject::tr("Macintosh 32-bit").toUtf8())
#	else
#		error "Your Macintosh platform is not properly defined. Could not determine architecture."
#	endif
#endif
#if PLATFORM == PLATFORM_LINUX
#	if defined(__LP64__) || defined(_LP64) || (_LINUX64)
#		define SYSTEMVERSION static_cast<const char*>(QObject::tr("Linux 64-bit").toUtf8())
#	elif defined (__i386__) || defined (__i386) || defined (i386) || (_LINUX32)
#		define SYSTEMVERSION static_cast<const char*>(QObject::tr("Linux 32-bit").toUtf8())
#	else
#		error "Your Linux platform is not properly defined. Could not determine architecture."
#	endif
#endif

// Additional Platform-specific functions
// None for now.

#endif
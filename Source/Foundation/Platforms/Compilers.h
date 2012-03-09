#ifndef COMPILERS_DEFINE_H
#define COMPILERS_DEFINE_H

#include "PlatformDefine.h"

#define COMPILER_MICROSOFT 0
#define COMPILER_GNU       1
#define COMPILER_BORLAND   2
#define COMPILER_INTEL     3

#ifdef _MSC_VER
#  define COMPILER COMPILER_MICROSOFT
#elif defined( __BORLANDC__ )
#  define COMPILER COMPILER_BORLAND
#elif defined( __INTEL_COMPILER )
#  define COMPILER COMPILER_INTEL
#elif defined( __GNUC__ )
#  define COMPILER COMPILER_GNU
#else
#  pragma error "FATAL ERROR: Unknown compiler."
#endif

// Disable Warnings & define missing items
#if COMPILER == COMPILER_MICROSOFT
	// Disable Errors
#	pragma warning( disable : 4267 )			// conversion from 'size_t' to 'int', possible loss of data

	// Missing Defines
#	if !defined (ssize_t)
#		if defined (_WIN64)
			typedef __int64 ssize_t;
#		else
			typedef __int32 ssize_t;
#		endif
#	endif
#endif

#endif
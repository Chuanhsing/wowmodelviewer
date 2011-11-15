#ifndef WMV_ENGINE_H
#define WMV_ENGINE_H

#ifdef WMV_ENGINE_EXPORTS
#define WMV_ENGINE_API Q_DECL_EXPORT
#else
#define WMV_ENGINE_API Q_DECL_IMPORT
#endif

// Logging Includes
#include "../Includes/QsLog/QsLog.h"
#include "../Includes/QsLog/QsLogDest.h"

// Logging
#ifdef _DEBUG
	// Debug mode gives us file names and line numbers in the log file
	#define QS_LOG_LINE_NUMBERS 1
#endif

#define QSLOGGER QsLogging::Logger::instance()

#endif
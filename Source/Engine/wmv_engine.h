#ifndef WMV_ENGINE_H
#define WMV_ENGINE_H

// Logging Includes
#include "../Foundation/QsLog/QsLog.h"
#include "../Foundation/QsLog/QsLogDest.h"

// Logging
#ifdef _DEBUG
	// Debug mode gives us file names and line numbers in the log file
	#define QS_LOG_LINE_NUMBERS 1
#endif

#define QSLOGGER QsLogging::Logger::instance()

//static QsLogging::Logger& QsLog_logger = QSLOGGER;

#endif
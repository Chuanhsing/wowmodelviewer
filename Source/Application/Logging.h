#ifndef WMV_LOGGING_H
#define WMV_LOGGING_H

#include "../Includes/QsLog/QsLog.h"
#include "../Includes/QsLog/QsLogDest.h"

// Logging Includes
#ifdef _DEBUG
	// Debug mode gives us file names and line numbers in the log file
	#define QS_LOG_LINE_NUMBERS 1
#endif

//static QsLogging::Logger& QsLog_logger = QsLogging::Logger::instance();

void initLogging(QString, bool append = false);

#endif
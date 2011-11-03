#include <qstring.h>
#include <qdir.h>
#include "Logging.h"

using namespace QsLogging;

void initLogging(QString logfile, bool append)
{
	QsLogging::Logger& logger = QsLogging::Logger::instance();
	logger.setLoggingLevel(QsLogging::TraceLevel);
	const QString sLogPath(logfile);
	QsLogging::Destination* fileDestination(
		QsLogging::DestinationFactory::MakeFileDestination(sLogPath,append) );
	QsLogging::Destination* debugDestination(
		QsLogging::DestinationFactory::MakeDebugOutputDestination() );
	logger.addDestination(debugDestination);
	logger.addDestination(fileDestination);

	QLOG_INFO() << "Post Init Test";
}
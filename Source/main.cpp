#include <QtGui/QApplication>
#include <QObject>
#include "globals.h"
#include "version.h"
#include "constants.h"
#include "application.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	QsLogging::Logger& logger = QsLogging::Logger::instance();
	logger.setLoggingLevel(QsLogging::TraceLevel);
	const QString sLogPath(QDir(app.applicationDirPath()).filePath(LOGTXTFILENAME));
	QsLogging::DestinationPtr fileDestination(
		QsLogging::DestinationFactory::MakeFileDestination(sLogPath) );
	QsLogging::DestinationPtr debugDestination(
		QsLogging::DestinationFactory::MakeDebugOutputDestination() );
	logger.addDestination(debugDestination.get());
	logger.addDestination(fileDestination.get());

	QLOG_INFO() << "Starting" << PROGRAMNAME << MAJORVERSION << BUILDVERSION;

	//QLog Demo Text, for reference.
	/*
	QLOG_INFO() << "Program started";
	QLOG_INFO() << "Built with Qt" << QT_VERSION_STR << "running on" << qVersion();

	QLOG_TRACE() << "Here's a" << QString("trace") << "message";
	QLOG_DEBUG() << "Here's a" << static_cast<int>(QsLogging::DebugLevel) << "message";
	QLOG_WARN()  << "Uh-oh!";
	qDebug() << "This message won't be picked up by the logger";
	QLOG_ERROR() << "An error has occurred";
	qWarning() << "Neither will this one";
	QLOG_FATAL() << "Fatal error!";

	This shows in the log (in debug mode) as:

	 INFO 2011-09-11T16:20:44.913 main.cpp @ 26 - Program started 
	 INFO 2011-09-11T16:20:44.913 main.cpp @ 27 - Built with Qt 4.7.3 running on 4.7.3 
	TRACE 2011-09-11T16:20:44.913 main.cpp @ 29 - Here's a "trace" message 
	DEBUG 2011-09-11T16:20:44.913 main.cpp @ 30 - Here's a 1 message 
	 WARN 2011-09-11T16:20:44.913 main.cpp @ 31 - Uh-oh! 
	ERROR 2011-09-11T16:20:44.913 main.cpp @ 33 - An error has occurred 
	FATAL 2011-09-11T16:20:44.913 main.cpp @ 35 - Fatal error! 

	*/

    WoWModelViewer w;
	g_WMV = &w;
	w.init();
    w.show();

    return app.exec();
}

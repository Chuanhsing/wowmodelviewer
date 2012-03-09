#include <QtGui/QApplication>
#include <qmessagebox.h>
#include <qstring.h>
#include <qdir.h>
#include "../Engine/kernel.h"
#include "InterfaceController.h"
#include "Settings.h"

using namespace WMVEngine;
using namespace QsLogging;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QString logfile = QDir(a.applicationDirPath()).filePath(LOGFILE_FILENAME);

	QsLogging::Logger& logger = QSLOGGER;
	logger.setLoggingLevel(QsLogging::TraceLevel);
	const QString sLogPath(logfile);
	QsLogging::DestinationPtr fileDestination(
		QsLogging::DestinationFactory::MakeFileDestination(sLogPath) );
	QsLogging::DestinationPtr debugDestination(
		QsLogging::DestinationFactory::MakeDebugOutputDestination() );
	logger.addDestination(debugDestination.get());
	logger.addDestination(fileDestination.get());

	QLOG_INFO() << "Logging Test, Application";

	// Scan for, and attempt to add any plugins...
	// hacked for now, just attempt the OpenGL plugin...
	//TheKernel.loadPlugin(QString("%1notaplugin").arg(PLUGIN_PATH));		// Debugging, Not a plugin
	//TheKernel.loadPlugin(QString("%1oldrenderversion").arg(PLUGIN_PATH));	// Debugging, Old rendering plugin
	//TheKernel.loadPlugin(QString("%1oldheaderversion").arg(PLUGIN_PATH));	// Debugging, Old Model Header Exporter plugin
	//TheKernel.loadPlugin(QString("%1oldmvmversion").arg(PLUGIN_PATH));	// Debugging, Old Model Viewer Model Exporter plugin
	TheKernel.loadPlugin(QString("%1renderer_opengl").arg(PLUGIN_PATH));	// OpenGL Plugin

	TheKernel.TestText = "Main File Corrected!";
	
	QLOG_INFO() << "Kernel Test Text:" << TheKernel.TestText;

	QLOG_INFO() << "--== Post Plugin Loading Test ==--";
	QLOG_INFO() << "Number of Display Drivers:" << TheKernel.getDisplayServer().getDisplayDriverCount();
	QLOG_INFO() << "--== End Post Plugin Loading Test ==--";

	// The interfaces.
	Interface_Cinema cine;// = Interface_Controller.getCinema();
	Interface_Viewer view;// = Interface_Controller.getViewer();
	cine.init();
	view.init();

	if (sWMVSettings.value("StartupInterfaceMode").toInt() == 1){
		cine.show();
		return a.exec();
	}else{
		view.show();
		return a.exec();
	}
}


#include "../Engine/kernel.h"
#include "../Interface_Viewer/Interface_Viewer.h"
#include <QtGui/QApplication>
#include <qmessagebox.h>

using namespace WMVEngine;
using namespace QsLogging;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QString logfile = QDir(a.applicationDirPath()).filePath(LOGFILE_FILENAME);

	initLogging(logfile);

	QLOG_INFO() << "Logging Test, Application";

//	QMessageBox b(QMessageBox::Information,"Application Logger Testing...",QString("Application Logging Level: %1").arg(QsLog_logger.loggingLevel()).toUtf8());
//	b.exec();

	// Scan for, and attempt to add any plugins...
	// hacked for now, just attempt the OpenGL plugin...
	TheKernel.loadPlugin(QString("%1/renderer_opengl").arg(PLUGIN_PATH));

	Interface_Viewer w;
	w.init();
	w.show();

	return a.exec();
}


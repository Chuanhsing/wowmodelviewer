#include <QString>
#include <qdir.h>
#include <QSharedPointer>
#include "OpenGL.h"
#include "../../../Engine/constants.h"

using namespace WMVEngine;
using namespace QsLogging;

#define LOGFILE_PLUGIN_RENDERER_OPENGL	"logs/plugin_renderer_opengl.txt"

// OpenGL graphics drver
class OpenGLDisplayDriver : public DisplayServer::DisplayDriver {
  public:
    // Destructor
    OPENGL_PLUGIN_API virtual ~OpenGLDisplayDriver() {}
    
    // Get name of the graphics driver
    OPENGL_PLUGIN_API virtual const QString &getName() const {
      static QString sName("OpenGL graphics driver");
      return sName;
    }

    // Create a renderer
    QSharedPointer<Renderer> createRenderer() {
		return QSharedPointer<Renderer>(new Renderer());
    }
};

// Retrieve the engine version we're going to expect
// This is the first function ran in the plugin.
extern "C" OPENGL_PLUGIN_API int getEngineVersion() {
	/*
	QsLog_logger.setLoggingLevel(QsLogging::TraceLevel);
	const QString sLogPath(QDir().filePath(LOGFILE_FILENAME));
	QsLogging::DestinationPtr fileDestination(
		QsLogging::DestinationFactory::MakeFileDestination(sLogPath,true) );
	QsLogging::DestinationPtr debugDestination(
		QsLogging::DestinationFactory::MakeDebugOutputDestination() );
	QsLog_logger.addDestination(debugDestination.get());
	QsLog_logger.addDestination(fileDestination.get());		
	*/

	//QLOG_INFO() << "Logging Test, OpenGL Plugin get Display Version";
	return DISPLAYSERVERVERSION;
}

// Tells us to register our functionality to an engine kernel
extern "C" OPENGL_PLUGIN_API void registerPlugin(Kernel &K) {
	//QLOG_INFO() << "Registering OpenGL Plugin...";
	K.getDisplayServer().addDisplayDriver(
		QSharedPointer<DisplayServer::DisplayDriver>(new OpenGLDisplayDriver())
	);
}

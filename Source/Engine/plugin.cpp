// I should mention that I copied a great deal of this code from a Windows-based tutorial project.
// I'm not sure what needs to be changed to make it more cross-platform compatible, so I'm hoping
// someone else will get it working/fix it so it works on Linux & Mac.

#include <stdexcept>
#include "plugin.h"

using namespace WMVEngine;
using namespace QsLogging;

// Loads the specified plugin as a dynamic library and locates the plugin's exported functions
Plugin::Plugin(const QString &sFilename) : m_GetEngineVersion(0),m_RegisterPlugin(0),m_DLLRefCount(0){
	QLOG_INFO() << "Loading Plugin file...";
	isLoaded = false;

	// Try to load the plugin as a dynamic library
	m_plugin.setFileName(sFilename);
	m_plugin.load();

	if (m_plugin.isLoaded()== false){
		QLOG_ERROR() << "Couldn't load" << sFilename;
		return;
	}

	try {
		m_GetEngineVersion = reinterpret_cast<fn_GetEngineVersion *>(m_plugin.resolve("getEngineVersion"));
		m_RegisterPlugin = reinterpret_cast<fn_RegisterPlugin *>(m_plugin.resolve("registerPlugin"));

		// If the functions aren't found, we're going to assume this is
		// a plain simple DLL and not one of our plugins
		if(!m_GetEngineVersion || ! m_RegisterPlugin){
			QLOG_ERROR() << QString("'%1' is not a valid plugin.").arg(sFilename);
			throw std::runtime_error(QString("'%1' is not a valid plugin.").arg(sFilename).toUtf8());
		}

		// Initialize a new DLL reference counter
		m_DLLRefCount = new size_t(1);
		isLoaded = true;
		QLOG_INFO() << sFilename << "is now loaded!";
	}
	catch(...) {
		QLOG_ERROR() << sFilename << "did not load.";
		m_plugin.unload();
		return;
	}
}

Plugin::Plugin(const Plugin &Other) :
	m_plugin(Other.m_plugin.fileName()),			// Plugin
	m_DLLRefCount(Other.m_DLLRefCount),				// Number of DLL references
	m_GetEngineVersion(Other.m_GetEngineVersion),	// Version Query Function
	m_RegisterPlugin(Other.m_RegisterPlugin)		// Plugin Registration Function
{
	if (m_plugin.isLoaded() == true){
		// Increase DLL reference counter
		QLOG_INFO() << "Increasing DLL Count...";
		++*m_DLLRefCount;
	}
}

Plugin Plugin::operator=(const Plugin &Other){
	QLOG_INFO() << "Logging Test, Plugin Creation 3";
	return Plugin(Other);
}

Plugin::~Plugin() {
	// Only unload the DLL if there are no more references to it
	if(!--*m_DLLRefCount) {
		isLoaded = false;
		m_plugin.unload();
		delete m_DLLRefCount;
	}
}
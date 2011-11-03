// I should mention that I copied a great deal of this code from a Windows-based tutorial project.
// I'm not sure what needs to be changed to make it more cross-platform compatible, so I'm hoping
// someone else will get it working/fix it so it works on Linux & Mac.

#include "plugin.h"

using namespace WMVEngine;
using namespace QsLogging;

// Loads the specified plugin as a dynamic library and locates the plugin's exported functions
Plugin::Plugin(const QString &sFilename) : m_GetEngineVersion(0),m_RegisterPlugin(0),m_DLLRefCount(0){
	QLOG_INFO() << "Logging Test, Plugin Creation 1";

	// Try to load the plugin as a dynamic library
	m_plugin.setFileName(sFilename);
	m_plugin.load();

	if (m_plugin.isLoaded()== false){
		// error message "Couldn't load " + sFilename
		return;
	}

	try {
		m_GetEngineVersion = reinterpret_cast<fn_GetEngineVersion *>(m_plugin.resolve("getEngineVersion"));
		m_RegisterPlugin = reinterpret_cast<fn_RegisterPlugin *>(m_plugin.resolve("registerPlugin"));

		// If the functions aren't found, we're going to assume this is
		// a plain simple DLL and not one of our plugins
		if(!m_GetEngineVersion || ! m_RegisterPlugin){
			//throw runtime_error(string("'") + sFilename + "' is not a valid plugin");
			throw;
		}

		// Initialize a new DLL reference counter
		m_DLLRefCount = new size_t(1);
	}
	catch(...) {
		m_plugin.unload();
		throw;
	}
}

Plugin::Plugin(const Plugin &Other) :
	m_plugin(Other.m_plugin.fileName()),			// Plugin
	m_DLLRefCount(Other.m_DLLRefCount),				// Number of DLL references
	m_GetEngineVersion(Other.m_GetEngineVersion),	// Version Query Function
	m_RegisterPlugin(Other.m_RegisterPlugin) {		// Plugin Registration Function
	QLOG_INFO() << "Logging Test, Plugin Creation 2";
		// Increase DLL reference counter
	++*m_DLLRefCount;
}

Plugin Plugin::operator=(const Plugin &Other){
	QLOG_INFO() << "Logging Test, Plugin Creation 3";
	return Plugin(Other);
}

Plugin::~Plugin() {
	QLOG_INFO() << "Logging Test, Plugin Destruction";
	// Only unload the DLL if there are no more references to it
	if(!--*m_DLLRefCount) {
		m_plugin.unload();
		delete m_DLLRefCount;
	}
}
#ifndef WMVENGINE_PLUGIN_H
#define WMVENGINE_PLUGIN_H

#include "wmv_engine.h"
#include <qstring.h>
#include <QLibrary>

namespace WMVEngine {

	class Kernel;

	class Plugin {
	public:
		// Initialize and load Plugin
		WMV_ENGINE_API Plugin(const QString &sFilename);
		// Copying Existing Plugin instance
		WMV_ENGINE_API Plugin(const Plugin &Other);
		// Unload the Plugin
		WMV_ENGINE_API ~Plugin();

		// Implementation

		// Ask the plugin for it's engine version
		WMV_ENGINE_API int getEngineVersion() const {
			return m_GetEngineVersion();
		}

		// Register the plugin to a Kernel
		WMV_ENGINE_API void registerPlugin(Kernel &k){
			QLOG_TRACE() << "Registering Plugin with Kernel...";
			m_RegisterPlugin(k);
		}
		
		// lazy copy
		WMV_ENGINE_API Plugin operator =(const Plugin &Other);

	private:
		// Signature for the version query function
		typedef int fn_GetEngineVersion();

		// Signature for the plugin's registration function
		typedef void fn_RegisterPlugin(Kernel &);

		QLibrary			m_plugin;				// Plugin
		size_t				*m_DLLRefCount;			// Number of DLL references
		fn_GetEngineVersion *m_GetEngineVersion;	// Version Query Function
		fn_RegisterPlugin	*m_RegisterPlugin;		// Plugin Registration Function

	};
}

#endif
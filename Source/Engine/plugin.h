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
		Plugin(const QString &sFilename);
		// Copying Existing Plugin instance
		Plugin(const Plugin &Other);
		// Unload the Plugin
		~Plugin();

		// Implementation

		// Ask the plugin for it's engine version
		int getEngineVersion() const {
			return m_GetEngineVersion();
		}

		// Register the plugin to a Kernel
		void registerPlugin(Kernel &k){
			QLOG_TRACE() << "Registering Plugin with Kernel...";
			m_RegisterPlugin(k);
		}
		
		// lazy copy
		Plugin operator =(const Plugin &Other);

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
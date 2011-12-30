////////////////////////////////////////////////////////////////////////////////////////
// By seperating the graphics/display system into it's own "plugin", we can reduce    //
// the code needed for WMV itself, and there-by limiting any bugs to a single plugin. //
// This will also be useful for platform-specific display drivers, such as DirectX    //
// for Windows.                                                                       //
////////////////////////////////////////////////////////////////////////////////////////

#ifndef WMVENGINE_DISPLAY_SERVER_H
#define WMVENGINE_DISPLAY_SERVER_H

#include "wmv_engine.h"
#include <QSharedPointer>
#include <qstring.h>
#include <qlist.h>

namespace WMVEngine {

// This should increment every time we change the DisplayServer or DisplayDriver classes
#define DISPLAYSERVERVERSION 1;

class Renderer {};		// Dummy class

class DisplayServer {
public:

	// Driver
	class DisplayDriver {
		public:
		// Destructor
		virtual ~DisplayDriver() {};
		// Get Name
		virtual const QString &getName() const = 0;
		// Create a Renderer
		virtual QSharedPointer<Renderer> createRenderer() = 0;
	};

	// Destructor
	~DisplayServer() {
		m_DisplayDrivers.clear();
	}

	// Allows plugins to add new display drivers
	void addDisplayDriver(QSharedPointer<DisplayDriver> DD){
		m_DisplayDrivers.push_back(DD.data());
	}

	// Get the total number of registers Display Drivers
	size_t getDisplayDriverCount() const {
		return m_DisplayDrivers.size();
	}

	// Access a driver by it's Index
	DisplayDriver &getDriver(size_t Index){
		return *m_DisplayDrivers.value((int)Index);
	}

private:
	// Vector List of installed Display Drivers
	typedef QList<DisplayDriver *> DisplayDriverVector;

	DisplayDriverVector m_DisplayDrivers;		// All of the graphics drivers!
};

} 	// End Namespace

#endif
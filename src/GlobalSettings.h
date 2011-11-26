/*
 * GlobalSettings.h
 *
 *  Created on: 26 nov. 2011
 *      Author: Jeromnimo
 */

#ifndef _GLOBALSETTINGS_H_
#define _GLOBALSETTINGS_H_

// Includes / class Declarations
//--------------------------------------------------------------------
// STL
#include <string>

// Wx

// Externals

// Other libraries

// Current library


// Namespaces used
//--------------------------------------------------------------------


// Class Declaration
//--------------------------------------------------------------------
#define GLOBALSETTINGS GlobalSettings::instance()

class GlobalSettings
{
	public :
		// Constants / Enums
		
		// Constructors 
	
		// Destructors
		~GlobalSettings();
	
		// Methods
		static GlobalSettings & instance();
		
		std::wstring appVersion(std::wstring a_prefix = std::wstring(L""));
		std::wstring appName();
		std::wstring buildName();
		std::wstring appTitle();

		// Members
		
	protected :
		// Constants / Enums
	
		// Constructors 

		// Destructors
	
		// Methods
		
		// Members
		
	private :
		// Constants / Enums
	
		// Constructors 
		GlobalSettings();
	
		// Destructors
	
		// Methods
		
		// Members
		static GlobalSettings * m_p_instance;

		int m_versionMajorNumber;
		int m_versionMinorNumber;
		int m_versionMajorRevNumber;
		int m_versionMinorRevNumber;
		std::wstring	m_appName;
		std::wstring	m_buildName;

		
		// friend class declarations
	
};

// static members definition
#ifdef _GLOBALSETTINGS_CPP_
	GlobalSettings * GlobalSettings::m_p_instance = 0;

#endif

#endif /* _GLOBALSETTINGS_H_ */

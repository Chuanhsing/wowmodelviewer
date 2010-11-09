#ifndef APP_H
#define APP_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#if _MSC_VER>=1400
	// This gives us Win XP style common controls in MSVC 8.0.
	#if defined _M_X64
		#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
	#elif defined _M_IA64
		#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
	#elif defined _M_IX86
		#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
	#else
		#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
	#endif
#endif

// Link to our libs.
#ifdef _WINDOWS
	#pragma message("     Adding library: opengl32.lib" ) 
	#pragma comment( lib, "opengl32.lib" )	// OpenGL API
	#pragma message("     Adding library: glu32.lib" ) 
	#pragma comment( lib, "glu32.lib" ) // OpenGL Utilities
	#define GLEW_BUILD
	#ifdef _DEBUG // GLEW lib
		#ifdef _WIN64
			#pragma message("     Adding library: glew64d.lib" ) 
			#pragma comment( lib, "glew64d.lib" )
		#else
			#pragma message("     Adding library: glew32d.lib" ) 
			#pragma comment( lib, "glew32d.lib" )
		#endif
	#else
		#ifdef _WIN64
			#pragma message("     Adding library: glew64.lib" ) 
			#pragma comment( lib, "glew64.lib" )
		#else
			#pragma message("     Adding library: glew32.lib" ) 
			#pragma comment( lib, "glew32.lib" )
		#endif
	#endif
	
	// FBX Libraries
	#ifdef _WIN64
		#ifdef _DEBUG
			#pragma message("     Adding library: fbxsdk_20113_amd64d.lib" ) 
			#pragma comment( lib, "fbxsdk_20113_amd64d.lib" )
		#else
			#pragma message("     Adding library: fbxsdk_20113_amd64.lib" ) 
			#pragma comment( lib, "fbxsdk_20113_amd64.lib" )
		#endif
	#else
		#ifdef _DEBUG
			#pragma message("     Adding library: fbxsdk_20113d.lib" ) 
			#pragma comment( lib, "fbxsdk_20113d.lib" )
		#else
			#pragma message("     Adding library: fbxsdk_20113.lib" ) 
			#pragma comment( lib, "fbxsdk_20113.lib" )
		#endif
	#endif
	

	#pragma message("     Adding library: uxtheme.lib" ) 
	#pragma comment( lib, "uxtheme.lib" ) // WinXP Theme Engine
	#pragma message("     Adding library: comctl32.lib" ) 
	#pragma comment( lib, "comctl32.lib" ) // Common Controls 32bit
	#pragma message("     Adding library: rpcrt4.lib" ) 
	#pragma comment( lib, "rpcrt4.lib" )
#endif

#ifdef _DEBUG
	#ifdef _WINDOWS

		#pragma comment( lib, "wxmsw28d_core.lib" )	// wxCore Debug Lib
		#pragma comment( lib, "wxmsw28d_adv.lib" )
		#pragma comment( lib, "wxmsw28d_qa.lib" )
		#pragma comment( lib, "wxmsw28d_aui.lib" )
		
		// Winsock 2
		#pragma comment( lib, "ws2_32.lib ") // This lib is required by wxbase28_net lib

	#elif _MAC

		#pragma comment( lib, "wxmac28d_core.lib" )
		#pragma comment( lib, "wxmac28d_adv.lib" )
		#pragma comment( lib, "wxmac28d_gl.lib" )
		#pragma comment( lib, "wxmac28d_qa.lib" )
		#pragma comment( lib, "wxmac28d_aui.lib" )

	#endif

	#pragma comment( lib, "wxzlibd.lib" )
	#pragma comment( lib, "wxregexd.lib" )
	#pragma comment( lib, "wxbase28d.lib" )
	#pragma comment( lib, "wxbase28d_net.lib" )
	#pragma comment( lib, "wxexpatd.lib" )
	#pragma comment( lib, "wxbase28d_xml.lib" )
	
	#pragma message("     Adding library: cximagecrtd.lib" ) 
	#pragma comment( lib, "cximagecrtd.lib" )	// cxImage

// release
#else
	
	#define NDEBUG			// Disables Asserts in release
	#define VC_EXTRALEAN	// Exclude rarely-used stuff from Windows headers
	#define WIN32_LEAN_AND_MEAN

	#ifdef _WINDOWS
		#pragma comment( lib, "wxmsw28_core.lib" )
		#pragma comment( lib, "wxmsw28_adv.lib" )
		#pragma comment( lib, "wxmsw28_qa.lib" )
		#pragma comment( lib, "wxmsw28_aui.lib" )
	
	#pragma comment( lib, "ws2_32.lib ") // This lib is required by wxbase28_net lib

	#pragma comment( lib, "wxzlib.lib" )
	#pragma comment( lib, "wxregex.lib" )
	#pragma comment( lib, "wxbase28.lib" )
	#pragma comment( lib, "wxbase28_net.lib" )
	#pragma comment( lib, "wxexpat.lib" )
	#pragma comment( lib, "wxbase28_xml.lib" )
	
	#pragma message("     Adding library: cximagecrt.lib" ) 
	#pragma comment( lib, "cximagecrt.lib" )
	#endif
#endif // _DEBUG

#ifndef _WINDOWS
	#include "../bin_support/Icons/wmv.xpm"
#endif

// headers
#include <wx/app.h>
//#include <wx/debugrpt.h>
#include <wx/log.h>
#include <wx/tokenzr.h>
#include <wx/dir.h>
#include <wx/aui/aui.h>

#include "util.h"
#include "globalvars.h"
#include "modelviewer.h"

// vars
const wxString langNames[] =
{
	_T("English"),
	//#if wxUSE_UNICODE
		_T("Korean"),
	//#endif
	_T("French"),
	_T("German"),
	//#if wxUSE_UNICODE
		_T("Chinese (Simplified)"),
		_T("Chinese (Traditional)"),
	//#endif
	_T("Spanish"),
	_T("Russian"),
};

static const wxLanguage langIds[] =
{
	wxLANGUAGE_ENGLISH,
	//#if wxUSE_UNICODE
		wxLANGUAGE_KOREAN,
	//#endif
	wxLANGUAGE_FRENCH,
	wxLANGUAGE_GERMAN,
	//#if wxUSE_UNICODE
		wxLANGUAGE_CHINESE_SIMPLIFIED,
		wxLANGUAGE_CHINESE_TRADITIONAL,
	//#endif
	wxLANGUAGE_SPANISH,
	wxLANGUAGE_RUSSIAN,
};

class WowModelViewApp : public wxApp
{
public:
    virtual bool OnInit();
	virtual int OnExit();
	virtual void OnUnhandledException();
	virtual void OnFatalException();

	//virtual bool OnExceptionInMainLoop();
	//virtual void HandleEvent(wxEvtHandler *handler, wxEventFunction func, wxEvent& event) const ; 

	bool LoadSettings();
	void SaveSettings();

	ModelViewer *frame;

	wxLocale locale;
	FILE *LogFile;
};

void searchMPQs();

#endif


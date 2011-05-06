#include "app.h"
#include "wx/image.h"
#include "wx/splash.h"
#include <wx/mstream.h>
#include "UserSkins.h"
#include "resource1.h"

/*	THIS IS OUR MAIN "START UP" FILE.
	App.cpp creates our wxApp class object.
	the wxApp initiates our program (takes over the role of main())
	When our wxApp loads,  it creates our ModelViewer class object,  
	which is a wxWindow.  From there ModelViewer object then creates
	our menu bar, character control, view control, filetree control, 
	animation control, and the canvas control (opengl).  Once those 
	controls are created it then loads saved variables from the config.ini
	file.  Then it proceeds	to create and open the MPQ archives,  creating
	a file list of the contents from all files within all of the opened mpq archives.

	I hope this gives some insight into the "program flow".
*/
/*
#ifdef _DEBUG
	#define new DEBUG_CLIENTBLOCK
#endif
*/

// tell wxwidgets which class is our app
IMPLEMENT_APP(WowModelViewApp)

//#include "globalvars.h"

bool WowModelViewApp::OnInit()
{
	frame = NULL;
	LogFile = NULL;
	wxSplashScreen* splash = NULL;

	wxImage::AddHandler( new wxPNGHandler);

	if (wxFile::Exists(wxT("Splash.png"))) {
		wxBitmap bitmap;
		if (bitmap.LoadFile(wxT("Splash.png"),wxBITMAP_TYPE_PNG) == false){
			wxMessageBox(wxT("Failed to load Splash Screen.\nPress OK to continue loading WMV."),wxT("Failure"));
			//return false;		// Used while debugging the splash screen.
		} else {
			splash = new wxSplashScreen(bitmap,
				wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
				6000, NULL, -1, wxDefaultPosition, wxDefaultSize,
				wxBORDER_NONE);
		}
		wxYield();
	}

	// Error & Logging settings
#ifndef _DEBUG
	#if wxUSE_ON_FATAL_EXCEPTION
		wxHandleFatalExceptions(true);
	#endif
#endif

	wxFileName fname(argv[0]);
	wxString userPath = fname.GetPath(wxPATH_GET_VOLUME)+SLASH+wxT("userSettings");
	wxFileName::Mkdir(userPath, 0777, wxPATH_MKDIR_FULL);

	// set the log file path.
	wxString logPath = userPath+SLASH+wxT("log.txt");

	LogFile = fopen(logPath.mb_str(), "w+");
	if (LogFile) {
		wxLog *logger = new wxLogStderr(LogFile);
		delete wxLog::SetActiveTarget(logger);
		wxLog::SetVerbose(false);
	}

	// Application Info
	SetVendorName(wxT("WoWModelViewer"));
	SetAppName(wxT("WoWModelViewer"));

	// Just a little header to start off the log file.
	wxLogMessage(wxString(wxT("Starting:\n") APP_TITLE wxT(" ") APP_VERSION wxT(" (") APP_BUILDNAME wxT(") ") APP_PLATFORM APP_ISDEBUG wxT("\n\n")));

	// set the config file path.
	cfgPath = userPath+SLASH+wxT("Config.ini");

	bool loadfail = LoadSettings();
	if (loadfail == true) {
		if (splash)
			splash->Show(false);
		return false;
	}

	// Load user skins 
	gUserSkins.LoadFile(userPath + SLASH + wxT("Skins.txt"));
	if (!gUserSkins.Loaded())
		wxLogMessage(wxT("Warning: Failed to load user skins"));


#ifdef _WINDOWS
	// This chunk of code is all related to locale translation (if a translation is available).
	// Only use locale for non-english?
	wxString fn = wxT("mo");
	fn = fn+SLASH+locales[interfaceID]+wxT(".mo");
	if (wxFileExists(fn))
	{
		locale.Init(langIds[interfaceID], wxLOCALE_CONV_ENCODING);
		
		wxLocale::AddCatalogLookupPathPrefix(wxT("mo"));
		//wxLocale::AddCatalogLookupPathPrefix(wxT(".."));

		//locale.AddCatalog(wxT("wowmodelview")); // Initialize the catalogs we'll be using
		locale.AddCatalog(locales[interfaceID]);
	}
#endif

	// Now create our main frame.
    frame = new ModelViewer();
    
	if (!frame) {
		//this->Close();
		if (splash)
			splash->Show(false);
		return false;
	}
	
	SetTopWindow(frame);
	/*
	There is a problem with drawing on surfaces that have previously not been showed.
	The error was 'GLXBadDrawable'.
	*/
	frame->Show(true);

	// Set the icon, different source location for the icon under Linux & Mac
	wxIcon icon;
#if defined (_WINDOWS)
	if (icon.LoadFile(wxT("mainicon"),wxBITMAP_TYPE_ICO_RESOURCE) == false)
		wxMessageBox(wxT("Failed to load Icon"),wxT("Failure"));
#elif defined (_LINUX)
	// This probably needs to be fixed...
	//if (icon.LoadFile(wxT("../bin_support/icon/wmv_xpm")) == false)
	//	wxMessageBox(wxT("Failed to load Icon"),wxT("Failure"));
#elif defined (_MAC)
	// Dunno what to do about Macs...
	//if (icon.LoadFile(wxT("../bin_support/icon/wmv.icns")) == false)
	//	wxMessageBox(wxT("Failed to load Icon"),wxT("Failure"));
#endif
	frame->SetIcon(icon);
	// --

	// Point our global vars at the correct memory location
	g_modelViewer = frame;
	g_canvas = frame->canvas;
	g_animControl = frame->animControl;
	g_charControl = frame->charControl;
	g_fileControl = frame->fileControl;

	frame->interfaceManager.Update();

	if (frame->canvas) {
		frame->canvas->Show(true);
		
		if (!frame->canvas->init)
			frame->canvas->InitGL();

		if (frame->lightControl)
			frame->lightControl->UpdateGL();
	}
	// --

	// TODO: Improve this feature and expand on it.
	// Command arguments
	wxString cmd;
	for (size_t i=0; i<argc; i++) {
		cmd = argv[i];

		if (cmd == wxT("-m")) {
			if (i+1 < argc) {
				i++;
				wxString fn(argv[i]);

				// Error check
				if (fn.Last() != '2') // Its not an M2 file, exit
					break;

				// Load the model
				frame->LoadModel(fn);
			}
		} else if (cmd == wxT("-mo")) {
			if (i+1 < argc) {
				i++;
				wxString fn(argv[i]);

				if (fn.Last() != '2') // Its not an M2 file, exit
					break;

				// If its a character model, give it some skin.
				// Load the model
				frame->LoadModel(fn);

				// Output the screenshot
				fn = wxT("ss_")+fn.AfterLast('\\').BeforeLast('.')+wxT(".png");
				frame->canvas->Screenshot(fn);
			}
		} else {
			wxString tmp = cmd.AfterLast('.');
			if (!tmp.IsNull() && !tmp.IsEmpty() && tmp.IsSameAs(wxT("chr"), false))
				frame->LoadChar(cmd);
		}
	}
	// -------
	// Load previously saved layout
	frame->LoadLayout();

	wxLogMessage(wxT("WoW Model Viewer successfully loaded!\n----\n"));
	
	if (splash) {
		// splash will auto closed after 6000ms
		// splash->Show(false);
		// splash->~wxSplashScreen();
	}
	
	return true;
}

void WowModelViewApp::OnFatalException()
{
	//wxApp::SetExitOnFrameDelete(false);

	/*
	wxDebugReport report;
    wxDebugReportPreviewStd preview;

	report.AddAll(wxDebugReport::Context_Exception);

    if (preview.Show(report))
        report.Process();
	*/

	if (frame != NULL) {
		frame->Destroy();
		frame = NULL;
	}
}

int WowModelViewApp::OnExit()
{
	SaveSettings();
	
	//if (frame != NULL)
	//	frame->Destroy();

//#ifdef _DEBUG
	//delete wxLog::SetActiveTarget(NULL);
	if (LogFile) {
		fclose(LogFile);
		//wxDELETE(LogFile);
		LogFile = NULL;
	}
//#endif

	CleanUp();

	//_CrtMemDumpAllObjectsSince( NULL );

	return 0;
}

/*
void WowModelViewApp::HandleEvent(wxEvtHandler *handler, wxEventFunction func, wxEvent& event) const 
{ 
    try 
    {        
        HandleEvent(handler, func, event); 
	} 
	catch(...) 
	{ 
		wxMessageBox(wxT("An error occured while handling an application event."), wxT("Execption in event handling"), wxOK | wxICON_ERROR); 
		throw; 
	} 
}
*/

void WowModelViewApp::OnUnhandledException() 
{ 
    //wxMessageBox(wxT("An unhandled exception was caught, the program will now terminate."), wxT("Unhandled Exception"), wxOK | wxICON_ERROR); 
	wxLogFatalError(wxT("An unhandled exception error has occured."));
}

namespace {
	long traverseLocaleMPQs(const wxString locales[], size_t localeCount, const wxString localeArchives[], size_t archiveCount, const wxString& gamePath)
	{
		long lngID = -1;

		for (size_t i = 0; i < localeCount; i++) {
			if (locales[i].IsEmpty())
				continue;
			wxString localePath = gamePath;

			localePath.Append(locales[i]);
			localePath.Append(wxT("/"));
			if (wxDir::Exists(localePath)) {
				wxArrayString localeMpqs;
				wxDir::GetAllFiles(localePath, &localeMpqs, wxEmptyString, wxDIR_FILES);

				for (size_t j = 0; j < archiveCount; j++) {
					for (size_t k = 0; k < localeMpqs.size(); k++) {
						wxString baseName = wxFileName(localeMpqs[k]).GetFullName();
						wxString neededMpq = wxString::Format(localeArchives[j], locales[i].c_str());

						if(baseName.CmpNoCase(neededMpq) == 0) {
							mpqArchives.Add(localeMpqs[k]);
						}
					}
				}

				lngID = (long)i;
				return lngID;
			}
		}

		return lngID;
	}
}

void searchMPQs(bool firstTime)
{
	if (mpqArchives.GetCount() > 0)
		return;

	bool bSearchCache = false;
	wxMessageDialog *dial = new wxMessageDialog(NULL, wxT("Do you want to search Cache dir?"),
		wxT("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
	if (wxID_YES == dial->ShowModal())
		bSearchCache = true;

	const wxString locales[] = {
		// sets 0
		wxT("enUS"), wxT("koKR"), wxT("frFR"), wxT("deDE"), 
		wxT("zhCN"), wxT("zhTW"), wxT("esES"), wxT("esMX"), wxT("ruRU"),
		wxT("jaJP"), wxT("ptBR"),
		// sets 1
		wxT("enGB"), wxEmptyString, wxEmptyString, wxEmptyString, 
		wxT("enCN"), wxT("enTW"), wxEmptyString, wxEmptyString, wxEmptyString,
		wxEmptyString, wxEmptyString
		};
	const int localeSets = WXSIZEOF(locales) / 2;
	const wxString defaultArchives[] = {wxT("patch-9.mpq"),wxT("patch-8.mpq"),wxT("patch-7.mpq"),wxT("patch-6.mpq"),
		wxT("patch-5.mpq"),wxT("patch-4.mpq"),wxT("patch-3.mpq"),wxT("patch-2.mpq"),wxT("patch.mpq"),wxT("alternate.mpq"),
		wxT("expansion3.mpq"),wxT("expansion2.mpq"),wxT("expansion1.mpq"),wxT("lichking.mpq"),wxT("expansion.mpq"),
		wxT("world.mpq"),wxT("sound.mpq"),wxT("art.mpq"),wxT("common-3.mpq"),wxT("common-2.mpq"), wxT("common.mpq")};
	const wxString localeArchives[] = {wxT("patch-%s-9.mpq"),wxT("patch-%s-8.mpq"),wxT("patch-%s-7.mpq"),
		wxT("patch-%s-6.mpq"),wxT("patch-%s-5.mpq"),wxT("patch-%s-4.mpq"),wxT("patch-%s-3.mpq"), wxT("patch-%s-2.mpq"), 
		wxT("patch-%s.mpq"), wxT("expansion3-locale-%s.mpq"), wxT("expansion2-locale-%s.mpq"), 
		wxT("expansion1-locale-%s.mpq"), wxT("lichking-locale-%s.mpq"), wxT("expansion-locale-%s.mpq"), 
		wxT("locale-%s.mpq"), wxT("base-%s.mpq")};

	// select avaiable locales, auto select user config locale
	wxArrayString avaiLocales;
	for (size_t i = 0; i < WXSIZEOF(locales); i++) {
		if (locales[i].IsEmpty())
			continue;
		wxString localePath = gamePath + wxT("Cache") + SLASH + locales[i];
		if (wxDir::Exists(localePath))
			avaiLocales.Add(locales[i]);
	}
	if (firstTime && avaiLocales.size() == 1) // only 1 locale
		langName = avaiLocales[0];
	else {
		// if user never select a locale, show all locales in data directory
		avaiLocales.Clear();
		for (size_t i = 0; i < WXSIZEOF(locales); i++) {
			if (locales[i].IsEmpty())
				continue;
			wxString localePath = gamePath + locales[i];

			if (wxDir::Exists(localePath))
				avaiLocales.Add(locales[i]);
		}
		if (avaiLocales.size() == 1) // only 1 locale
			langName = avaiLocales[0];
		else
			langName = wxGetSingleChoice(wxT("Please select a Locale:"), wxT("Locale"), avaiLocales);
	}

	// search Partial MPQs
	wxArrayString baseMpqs;
	wxDir::GetAllFiles(gamePath, &baseMpqs, wxEmptyString, wxDIR_FILES);
	for (size_t j = 0; j < baseMpqs.size(); j++) {
		if (baseMpqs[j].Contains(wxT("oldworld")))
			continue;
		wxString baseName = wxFileName(baseMpqs[j]).GetFullName();
		wxString cmpName = wxT("wow-update-");
		if (baseName.StartsWith(cmpName) && baseName.AfterLast('.').CmpNoCase(wxT("mpq")) == 0) {
			bool bFound = false;
			for(size_t i = 0; i<mpqArchives.size(); i++) {
				wxString archiveName = wxFileName(mpqArchives[i]).GetFullName();
				if (!archiveName.AfterLast(SLASH).StartsWith(cmpName))
					continue;
				int ver = wxAtoi(archiveName.BeforeLast('.').AfterLast('-'));
				int bver = wxAtoi(baseName.BeforeLast('.').AfterLast('-'));
				if (bver > ver) {
					mpqArchives.Insert(baseMpqs[j], i);
					bFound = true;
					break;
				}		
			}
			if (bFound == false)
				mpqArchives.Add(baseMpqs[j]);

			wxLogMessage(wxT("- Found Partial MPQ archive: %s"), baseMpqs[j].Mid(gamePath.Len()).c_str());
		}
	}

	// search Partial MPQs inside langName directory
	wxDir::GetAllFiles(gamePath+langName, &baseMpqs, wxEmptyString, wxDIR_FILES);
	for (size_t j = 0; j < baseMpqs.size(); j++) {
		if (baseMpqs[j].Contains(wxT("oldworld")))
			continue;
		wxString baseName = wxFileName(baseMpqs[j]).GetFullName();
		wxString cmpName = wxT("wow-update-")+langName;
		if (baseName.StartsWith(cmpName) && baseName.AfterLast('.').CmpNoCase(wxT("mpq")) == 0) {
			bool bFound = false;
			for(size_t i = 0; i<mpqArchives.size(); i++) {
				wxString archiveName = wxFileName(mpqArchives[i]).GetFullName();
				if (!archiveName.StartsWith(wxT("wow-update-"))) // compare to all wow-update-
					continue;
				int ver = wxAtoi(archiveName.BeforeLast('.').AfterLast('-'));
				int bver = wxAtoi(baseName.BeforeLast('.').AfterLast('-'));
				if (bver > ver) {
					mpqArchives.Insert(baseMpqs[j], i);
					bFound = true;
					break;
				}		
			}
			if (bFound == false)
				mpqArchives.Add(baseMpqs[j]);

			wxLogMessage(wxT("- Found Partial MPQ archive: %s"), baseMpqs[j].Mid(gamePath.Len()).c_str());
		}
	}

	// search patch-base MPQs
	wxArrayString baseCacheMpqs;
	wxDir::GetAllFiles(gamePath+wxT("Cache"), &baseCacheMpqs, wxEmptyString, wxDIR_FILES);
	for (size_t j = 0; j < baseCacheMpqs.size(); j++) {
		if (bSearchCache == false)
			continue;
		if (baseCacheMpqs[j].Contains(wxT("oldworld")))
			continue;
		wxString baseName = baseCacheMpqs[j];
		wxString fullName = wxFileName(baseName).GetFullName();
		wxString cmpName = wxT("patch-base-");
		if (fullName.StartsWith(cmpName) && fullName.AfterLast('.').CmpNoCase(wxT("mpq")) == 0) {
			bool bFound = false;
			for(size_t i = 0; i<mpqArchives.size(); i++) {
				if (!mpqArchives[i].AfterLast(SLASH).StartsWith(cmpName))
					continue;
				int ver = wxAtoi(mpqArchives[i].BeforeLast('.').AfterLast('-'));
				int bver = wxAtoi(fullName.BeforeLast('.').AfterLast('-'));
				if (bver > ver) {
#if 1 // Use lastest archive only
					mpqArchives[i] = baseName;
#else
					mpqArchives.Insert(baseName, i);
#endif
					bFound = true;
					break;
				}		
			}
			if (bFound == false)
				mpqArchives.Add(baseName);

			wxLogMessage(wxT("- Found Patch Base MPQ archive: %s"), baseName.Mid(gamePath.Len()).c_str());
		}
	}
	baseCacheMpqs.Clear();

	// search base cache locale MPQs
	wxArrayString baseCacheLocaleMpqs;
	wxDir::GetAllFiles(gamePath+wxT("Cache")+SLASH+langName, &baseCacheLocaleMpqs, wxEmptyString, wxDIR_FILES);
	for (size_t j = 0; j < baseCacheLocaleMpqs.size(); j++) {
		if (bSearchCache == false)
			continue;
		if (baseCacheLocaleMpqs[j].Contains(wxT("oldworld")))
			continue;
		wxString baseName = baseCacheLocaleMpqs[j];
		wxString fullName = wxFileName(baseName).GetFullName();
		wxString cmpName = wxT("patch-")+langName+wxT("-");
		if (fullName.StartsWith(cmpName) && fullName.AfterLast('.').CmpNoCase(wxT("mpq")) == 0) {
			bool bFound = false;
			for(size_t i = 0; i<mpqArchives.size(); i++) {
				if (!mpqArchives[i].AfterLast(SLASH).StartsWith(cmpName))
					continue;
				int ver = wxAtoi(mpqArchives[i].BeforeLast('.').AfterLast('-'));
				int bver = wxAtoi(fullName.BeforeLast('.').AfterLast('-'));
				if (bver > ver) {
#if 1 // Use lastest archive only
					mpqArchives[i] = baseName;
#else
					mpqArchives.Insert(baseName, i);
#endif
					bFound = true;
					break;
				}		
			}
			if (bFound == false)
				mpqArchives.Add(baseName);

			wxLogMessage(wxT("- Found Patch Base Locale MPQ archive: %s"), baseName.Mid(gamePath.Len()).c_str());
		}
	}
	baseCacheLocaleMpqs.Clear();

	// default archives
	for (size_t i = 0; i < WXSIZEOF(defaultArchives); i++) {
		//wxLogMessage(wxT("Searching for MPQ archive %s..."), defaultArchives[i].c_str());

		for (size_t j = 0; j < baseMpqs.size(); j++) {
			wxString baseName = wxFileName(baseMpqs[j]).GetFullName();
			if(baseName.CmpNoCase(defaultArchives[i]) == 0) {
				mpqArchives.Add(baseMpqs[j]);

				wxLogMessage(wxT("- Found MPQ archive: %s"), baseMpqs[j].Mid(gamePath.Len()).c_str());
				if (baseName.CmpNoCase(wxT("alternate.mpq")))
					bAlternate = true;
			}
		}
	}

	// add locale files
	for (size_t i = 0; i < WXSIZEOF(locales); i++) {
		if (locales[i] == langName) {
			wxString localePath = gamePath;

			localePath.Append(locales[i]);
			localePath.Append(wxT("/"));
			if (wxDir::Exists(localePath)) {
				wxArrayString localeMpqs;
				wxDir::GetAllFiles(localePath, &localeMpqs, wxEmptyString, wxDIR_FILES);

				for (size_t j = 0; j < WXSIZEOF(localeArchives); j++) {
					for (size_t k = 0; k < localeMpqs.size(); k++) {
						wxString baseName = wxFileName(localeMpqs[k]).GetFullName();
						wxString neededMpq = wxString::Format(localeArchives[j], locales[i].c_str());

						if(baseName.CmpNoCase(neededMpq) == 0) {
							mpqArchives.Add(localeMpqs[k]);
						}
					}
				}
			}

			langID = i % localeSets;
			break;
		}
	}

	if (langID == -1) {
		langID = traverseLocaleMPQs(locales, WXSIZEOF(locales), localeArchives, WXSIZEOF(localeArchives), gamePath);
		if (langID != -1)
			langID = langID % localeSets;
	}

}

bool WowModelViewApp::LoadSettings()
{
	wxString tmp;
	// Application Config Settings
	wxFileConfig *pConfig = new wxFileConfig(wxT("Global"), wxEmptyString, cfgPath, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
	
	// Graphic / Video display settings
	pConfig->SetPath(wxT("/Graphics"));
	pConfig->Read(wxT("FSAA"), &video.curCap.aaSamples, 0);
	pConfig->Read(wxT("AccumulationBuffer"), &video.curCap.accum, 0);
	pConfig->Read(wxT("AlphaBits"), &video.curCap.alpha, 0);
	pConfig->Read(wxT("ColourBits"), &video.curCap.colour, 24);
	pConfig->Read(wxT("DoubleBuffer"), (bool*)&video.curCap.doubleBuffer, 1);	// True
#ifdef _WINDOWS
	pConfig->Read(wxT("HWAcceleration"), &video.curCap.hwAcc, WGL_FULL_ACCELERATION_ARB);
#endif
	pConfig->Read(wxT("SampleBuffer"), (bool*)&video.curCap.sampleBuffer, 0);	// False
	pConfig->Read(wxT("StencilBuffer"), &video.curCap.stencil, 0);
	pConfig->Read(wxT("ZBuffer"), &video.curCap.zBuffer, 16);

	// Application locale info
	pConfig->SetPath(wxT("/Locale"));
	pConfig->Read(wxT("LanguageID"), &langID, -1);
	pConfig->Read(wxT("LanguageName"), &langName, wxEmptyString);
	pConfig->Read(wxT("InterfaceID"), &interfaceID, -1);

	// Application settings
	pConfig->SetPath(wxT("/Settings"));
	pConfig->Read(wxT("Path"), &gamePath, wxEmptyString);
	pConfig->Read(wxT("TOCVersion"), &gameVersion, 0);

	pConfig->Read(wxT("UseLocalFiles"), &useLocalFiles, false);
	pConfig->Read(wxT("SSCounter"), &ssCounter, 100);
	//pConfig->Read(wxT("AntiAlias"), &useAntiAlias, true);
	//pConfig->Read(wxT("DisableHWAcc"), &disableHWAcc, false);
	pConfig->Read(wxT("DefaultFormat"), &imgFormat, 0);

	pConfig->Read(wxT("PerferedExporter"), &Perfered_Exporter, -1);
	pConfig->Read(wxT("ModelExportInitOnly"), &modelExportInitOnly, true);
	pConfig->Read(wxT("ModelExportPreserveDirs"), &modelExport_PreserveDir, true);
	pConfig->Read(wxT("ModelExportUseWMVPosRot"), &modelExport_UseWMVPosRot, false);
	pConfig->Read(wxT("ModelExportScaleToRealWorld"), &modelExport_ScaleToRealWorld, false);

	pConfig->Read(wxT("ModelExportLWPreserveDirs"), &modelExport_LW_PreserveDir, true);
	pConfig->Read(wxT("ModelExportLWAlwaysWriteSceneFile"), &modelExport_LW_AlwaysWriteSceneFile, false);
	pConfig->Read(wxT("ModelExportLWExportLights"), &modelExport_LW_ExportLights, true);
	pConfig->Read(wxT("ModelExportLWExportDoodads"), &modelExport_LW_ExportDoodads, true);
	pConfig->Read(wxT("ModelExportLWExportCameras"), &modelExport_LW_ExportCameras, true);
	pConfig->Read(wxT("ModelExportLWExportBones"), &modelExport_LW_ExportBones, true);

	pConfig->Read(wxT("ModelExportLWDoodadsAs"), &modelExport_LW_DoodadsAs, 0);

	pConfig->Read(wxT("ModelExportM3BoundScale"), &tmp, wxT("0.5"));
	modelExport_M3_BoundScale = wxAtof(tmp);
	pConfig->Read(wxT("ModelExportM3SphereScale"), &tmp, wxT("0.5"));
	modelExport_M3_SphereScale = wxAtof(tmp);
	pConfig->Read(wxT("ModelExportM3TexturePath"), &modelExport_M3_TexturePath, wxEmptyString);


	// Data path and mpq archive stuff
	wxString archives;
	pConfig->Read(wxT("MPQFiles"), &archives);
	
	wxStringTokenizer strToken(archives, wxT(";"), wxTOKEN_DEFAULT);
	while (strToken.HasMoreTokens()) {
		mpqArchives.Add(strToken.GetNextToken());
	}

	if (gamePath.IsEmpty() || !wxDirExists(gamePath)) {
		getGamePath();
		mpqArchives.Clear();
	}

	if (gamePath.Last() != SLASH)
		gamePath.Append(SLASH);

	if (mpqArchives.GetCount()==0) {
		searchMPQs(true);
	}

	// if we can't search any mpqs
	if (mpqArchives.GetCount()==0) {
		wxLogMessage(wxT("World of Warcraft Data Directory Not Found. Returned GamePath: %s"),gamePath.c_str());
		wxMessageDialog *dial = new wxMessageDialog(NULL, wxT("Fatal Error: Could not find your World of Warcraft Data folder."), wxT("World of Warcraft Not Found"), wxOK | wxICON_ERROR);
		dial->ShowModal();
		return true;
	}

	// Clear our ini file config object
	wxDELETE( pConfig );

    if (langID == -1) {
        // the arrays should be in sync
        wxCOMPILE_TIME_ASSERT(WXSIZEOF(langNames) == WXSIZEOF(langIds), LangArraysMismatch);
        langID = wxGetSingleChoiceIndex(wxT("Please select a language:"), wxT("Language"), WXSIZEOF(langNames), langNames);

		SaveSettings();
	}

	// initial interfaceID as langID
	if (interfaceID == -1)
		interfaceID = langID;

	// initial langOffset
	if (langOffset == -1) {
		// gameVersion VERSION_CATACLYSM remove all other language strings
		if (gameVersion >= VERSION_CATACLYSM)
			langOffset = 0;
		else
			langOffset = langID;
	}
	return false;
}

void WowModelViewApp::SaveSettings()
{
	// Application Config Settings
	wxFileConfig *pConfig = new wxFileConfig(wxT("Global"), wxEmptyString, cfgPath, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
	
	pConfig->SetPath(wxT("/Locale"));
	pConfig->Write(wxT("LanguageID"), langID);
	pConfig->Write(wxT("LanguageName"), langName);
	pConfig->Write(wxT("InterfaceID"), interfaceID);

	pConfig->SetPath(wxT("/Settings"));
	pConfig->Write(wxT("Path"), gamePath);
	pConfig->Write(wxT("TOCVersion"), gameVersion);
	pConfig->Write(wxT("UseLocalFiles"), useLocalFiles);
	pConfig->Write(wxT("SSCounter"), ssCounter);
	//pConfig->Write(wxT("AntiAlias"), useAntiAlias);
	//pConfig->Write(wxT("DisableHWAcc"), disableHWAcc);
	pConfig->Write(wxT("DefaultFormat"), imgFormat);

	pConfig->Write(wxT("PerferedExporter"), Perfered_Exporter);
	pConfig->Write(wxT("ModelExportInitOnly"), modelExportInitOnly);
	pConfig->Write(wxT("ModelExportPreserveDirs"), modelExport_PreserveDir);
	pConfig->Write(wxT("ModelExportUseWMVPosRot"), modelExport_UseWMVPosRot);
	pConfig->Write(wxT("ModelExportScaleToRealWorld"), modelExport_ScaleToRealWorld);

	pConfig->Write(wxT("ModelExportLWPreserveDirs"), modelExport_LW_PreserveDir);
	pConfig->Write(wxT("ModelExportLWAlwaysWriteSceneFile"), modelExport_LW_AlwaysWriteSceneFile);
	pConfig->Write(wxT("ModelExportLWExportLights"), modelExport_LW_ExportLights);
	pConfig->Write(wxT("ModelExportLWExportDoodads"), modelExport_LW_ExportDoodads);
	pConfig->Write(wxT("ModelExportLWExportCameras"), modelExport_LW_ExportCameras);
	pConfig->Write(wxT("ModelExportLWExportBones"), modelExport_LW_ExportBones);

	pConfig->Write(wxT("ModelExportLWDoodadsAs"), modelExport_LW_DoodadsAs);

	pConfig->Write(wxT("ModelExportM3BoundScale"), wxString::Format(wxT("%0.2f"), modelExport_M3_BoundScale));
	pConfig->Write(wxT("ModelExportM3SphereScale"), wxString::Format(wxT("%0.2f"), modelExport_M3_SphereScale));
	pConfig->Write(wxT("ModelExportM3TexturePath"), modelExport_M3_TexturePath);

	wxString archives;

	for (size_t i=0; i<mpqArchives.GetCount(); i++) {
		archives.Append(mpqArchives[i]);
		archives.Append(wxT(";"));
	}

	pConfig->Write(wxT("MPQFiles"), archives);

	// Clear our ini file config object
	wxDELETE( pConfig );
}



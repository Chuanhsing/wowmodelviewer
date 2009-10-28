#include "app.h"


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


	// Error & Logging settings
#ifndef _DEBUG
	#if wxUSE_ON_FATAL_EXCEPTION
		wxHandleFatalExceptions(true);
	#endif
#endif

	wxFileName fname(argv[0]);
	wxString userPath = fname.GetPath(wxPATH_GET_VOLUME)+"\\userSettings";
	wxFileName::Mkdir(userPath, 0777, wxPATH_MKDIR_FULL);

	// set the log file path.
	wxString logPath = userPath+"\\log.txt";
	LogFile = fopen(logPath.c_str(), "w+");
	if (LogFile) {
		wxLog *logger = new wxLogStderr(LogFile);
		delete wxLog::SetActiveTarget(logger);
		wxLog::SetVerbose(false);
	}

	// Application Info
	SetVendorName(_T("WoWModelViewer"));
	SetAppName(_T("WoWModelViewer"));

	// Just a little header to start off the log file.
	wxLogMessage(wxString(_T("Starting:\n") APP_TITLE _T(" ") APP_VERSION _T("\n\n")));
	
	// set the config file path.
	cfgPath = userPath+"\\Config.ini";

	LoadSettings();

/*
#ifdef _WIN32
	// This chunk of code is all related to locale translation (if a translation is available).
	// Only use locale for non-english?
	if (langID != -1 && langID != 1) {
		locale.Init(langIds[langID], wxLOCALE_CONV_ENCODING);
		
		//wxLocale::AddCatalogLookupPathPrefix(_T("."));
		//wxLocale::AddCatalogLookupPathPrefix(_T(".."));

		locale.AddCatalog(_T("wowmodelview")); // Initialize the catalogs we'll be using
	}
#endif
*/

	// Now create our main frame.
    frame = new ModelViewer();
    
	if (!frame) {
		//this->Close();
		return false;
	}
	
	SetTopWindow(frame);

	// Set the icon, different source location for the icon under linux
	wxIcon icon(mondrian_xpm);
	frame->SetIcon(icon);
	// --

	// Point our global vars at the correct memory location
	g_modelViewer = frame;
	g_canvas = frame->canvas;
	g_animControl = frame->animControl;
	g_charControl = frame->charControl;

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
	for (int i=0; i<argc; i++) {
		cmd = argv[i];

		if (cmd == "-m") {
			if (i+1 < argc) {
				i++;
				wxString fn(argv[i]);

				// Error check
				if (fn.Last() != '2') // Its not an M2 file, exit
					break;

				// Load the model
				frame->LoadModel(fn);
			}
		} else if (cmd == "-mo") {
			if (i+1 < argc) {
				i++;
				wxString fn(argv[i]);

				if (fn.Last() != '2') // Its not an M2 file, exit
					break;

				// If its a character model, give it some skin.
				// Load the model
				frame->LoadModel(fn);

				// Output the screenshot
				fn = fn.AfterLast('\\');
				fn = fn.BeforeLast('.');
				fn.Prepend("ss_");
				fn.Append(".png");
				frame->canvas->Screenshot(fn);
			}
		} else {
			wxString tmp = cmd.AfterLast('.');
			if (!tmp.IsNull() && !tmp.IsEmpty() && tmp.IsSameAs("chr", false))
				frame->LoadChar(cmd.fn_str());
		}
	}
	// -------

	// Load previously saved layout
	frame->LoadLayout();

	wxLogMessage(_T("WoW Model Viewer successfully loaded!\n----\n"));
	
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
		wxMessageBox(_T("An error occured while handling an application event."), _T("Execption in event handling"), wxOK | wxICON_ERROR); 
		throw; 
	} 
}
*/

void WowModelViewApp::OnUnhandledException() 
{ 
    //wxMessageBox(_T("An unhandled exception was caught, the program will now terminate."), _T("Unhandled Exception"), wxOK | wxICON_ERROR); 
	wxLogFatalError(_T("An unhandled exception error has occured."));
} 

void WowModelViewApp::LoadSettings()
{
	// Application Config Settings
	wxFileConfig *pConfig = new wxFileConfig("Global", wxEmptyString, cfgPath, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
	
	// Graphic / Video display settings
	pConfig->SetPath(_T("/Graphics"));
	pConfig->Read(_T("FSAA"), &video.curCap.aaSamples, 0);
	pConfig->Read(_T("AccumulationBuffer"), &video.curCap.accum, 0);
	pConfig->Read(_T("AlphaBits"), &video.curCap.alpha, 0);
	pConfig->Read(_T("ColourBits"), &video.curCap.colour, 24);
	pConfig->Read(_T("DoubleBuffer"), (bool*)&video.curCap.doubleBuffer, 1);	// True
	pConfig->Read(_T("HWAcceleration"), &video.curCap.hwAcc, WGL_FULL_ACCELERATION_ARB);
	pConfig->Read(_T("SampleBuffer"), (bool*)&video.curCap.sampleBuffer, 0);	// False
	pConfig->Read(_T("StencilBuffer"), &video.curCap.stencil, 0);
	pConfig->Read(_T("ZBuffer"), &video.curCap.zBuffer, 16);

	// Application locale info
	pConfig->SetPath(_T("/Locale"));
	pConfig->Read(_T("LanguageID"), &langID, -1);

	// Application settings
	pConfig->SetPath(_T("/Settings"));
	pConfig->Read(_T("Path"), &gamePath, "");

	pConfig->Read(_T("UseLocalFiles"), &useLocalFiles, false);
	pConfig->Read(_T("SSCounter"), &ssCounter, 100);
	//pConfig->Read(_T("AntiAlias"), &useAntiAlias, true);
	//pConfig->Read(_T("DisableHWAcc"), &disableHWAcc, false);
	pConfig->Read(_T("DefaultFormat"), &imgFormat, 0);


	// Data path and mpq archive stuff
	wxString archives;
	pConfig->Read(_T("MPQFiles"), &archives);
	
	wxStringTokenizer strToken(archives, _T(";"), wxTOKEN_DEFAULT);
	while (strToken.HasMoreTokens()) {
		mpqArchives.Add(strToken.GetNextToken());
	}

	if (gamePath.IsEmpty())
		getGamePath();

#ifdef _WIN32
	if (gamePath.Last() != '\\')
		gamePath.Append("\\", 1);
#else // Linux
	if (gamePath.Last() != '/')
		gamePath.Append("/", 1);
#endif
	
	if (mpqArchives.GetCount()==0) {
		//enUS(enGB), koKR, frFR, deDE, zhCN, zhTW, esES, ruRU
		const wxString locales[] = {_T("enUS"), _T("koKR"), _T("frFR"), _T("deDE"), _T("zhCN"),  _T("zhTW"),  _T("esES"),  _T("ruRU")};
		const wxString locales2[] = {_T("enGB")};

#ifdef WotLK
		const wxString defaultArchives[] = {_T("Patch-2.MPQ"),_T("Patch.MPQ"), _T("Lichking.MPQ"), _T("Expansion.MPQ"),_T("Common-2.MPQ"), _T("Common.MPQ")};
		const wxString localeArchives[] = {_T("Patch"), _T("Lichking-Locale"), _T("expansion-locale"), _T("Locale"), _T("Base")};
#else
		const wxString defaultArchives[] = {_T("Patch-2.MPQ"),_T("Patch.MPQ"), _T("Expansion.MPQ"), _T("Common.MPQ")};
		const wxString localeArchives[] = {_T("Patch"), _T("expansion-locale"), _T("Locale"), _T("Base")};
#endif

		for (size_t i=0; i<WXSIZEOF(defaultArchives); i++) {
			wxString mpqFile = gamePath;
			mpqFile.Append(defaultArchives[i]);
#if 0 // WotLK
			if (defaultArchives[i] == _T("Patch-2.MPQ"))
				continue;
#endif
			if (wxFile::Exists(mpqFile.c_str()))
				mpqArchives.Add(mpqFile);
		}
		
		for (size_t i=0; i<WXSIZEOF(locales); i++) {
			wxString localePath = gamePath;
			
			localePath.Append(locales[i]);
			localePath.Append("\\");
			if (wxDir::Exists(localePath)) {
				if (1) {
					wxString mpqFile = localePath;
					mpqFile.Append("Patch-");
					mpqFile.Append(locales[i]);
					mpqFile.Append("-2.MPQ");
					if (wxFile::Exists(mpqFile.c_str()))
						mpqArchives.Add(mpqFile);
				}
				for (size_t j=0; j<WXSIZEOF(localeArchives); j++) {
					wxString mpqFile = localePath;
					mpqFile.Append(localeArchives[j]);
					mpqFile.Append("-");
					mpqFile.Append(locales[i]);
					mpqFile.Append(".MPQ");
					if (wxFile::Exists(mpqFile.c_str()))
						mpqArchives.Add(mpqFile);
				}
				langID = (long)i;
			}
		}
		for (size_t i=0; i<WXSIZEOF(locales2); i++) {
			wxString localePath = gamePath;
			
			localePath.Append(locales2[i]);
			localePath.Append("\\");
			if (wxDir::Exists(localePath)) {
				if (1) {
					wxString mpqFile = localePath;
					mpqFile.Append("Patch-");
					mpqFile.Append(locales2[i]);
					mpqFile.Append("-2.MPQ");
					if (wxFile::Exists(mpqFile.c_str()))
						mpqArchives.Add(mpqFile);
				}
				for (size_t j=0; j<WXSIZEOF(localeArchives); j++) {
					wxString mpqFile = localePath;
					mpqFile.Append(localeArchives[j]);
					mpqFile.Append("-");
					mpqFile.Append(locales2[i]);
					mpqFile.Append(".MPQ");
					if (wxFile::Exists(mpqFile.c_str()))
						mpqArchives.Add(mpqFile);
				}
				langID = (long)i;
			}
		}

	}
	// -------
	

	// Clear our ini file config object
	wxDELETE( pConfig );

    if (langID == -1) {
        // the arrays should be in sync
        wxCOMPILE_TIME_ASSERT(WXSIZEOF(langNames) == WXSIZEOF(langIds), LangArraysMismatch);
        langID = wxGetSingleChoiceIndex(_T("Please select a language:"), _T("Language"), WXSIZEOF(langNames), langNames);

		SaveSettings();
	}
}

void WowModelViewApp::SaveSettings()
{
	// Application Config Settings
	wxFileConfig *pConfig = new wxFileConfig("Global", wxEmptyString, cfgPath, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
	
	pConfig->SetPath(_T("/Locale"));
	pConfig->Write(_T("LanguageID"), langID);

	pConfig->SetPath(_T("/Settings"));
	pConfig->Write(_T("Path"), gamePath);
	pConfig->Write(_T("UseLocalFiles"), useLocalFiles);
	pConfig->Write(_T("SSCounter"), ssCounter);
	//pConfig->Write(_T("AntiAlias"), useAntiAlias);
	//pConfig->Write(_T("DisableHWAcc"), disableHWAcc);
	pConfig->Write(_T("DefaultFormat"), imgFormat);

	wxString archives;

	for (size_t i=0; i<mpqArchives.GetCount(); i++) {
		archives.Append(mpqArchives[i]);
		archives.Append(_T(";"));
	}

	pConfig->Write(_T("MPQFiles"), archives);

	// Clear our ini file config object
	wxDELETE( pConfig );
}



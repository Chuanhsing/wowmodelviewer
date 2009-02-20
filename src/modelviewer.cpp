
#include "modelviewer.h"
#include "globalvars.h"

// default colour values
const float def_ambience[4] = {0.0f, 0.0f, 0.0f, 1.0f};
const float def_diffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};
const float def_emission[4] = {0.0f, 0.0f, 0.0f, 1.0f};
const float def_specular[4] = {1.0f, 1.0f, 1.0f, 1.0f};


typedef std::pair<wxTreeItemId, std::string> TreeStackItem;
typedef std::vector<TreeStackItem> TreeStack;

// Class event handler/importer
IMPLEMENT_CLASS(ModelViewer, wxFrame)

BEGIN_EVENT_TABLE(ModelViewer, wxFrame)
	EVT_CLOSE(ModelViewer::OnClose)
	//EVT_SIZE(ModelViewer::OnSize)

	// File menu
	EVT_MENU(ID_FILE_NPC, ModelViewer::OnCharToggle)
	EVT_MENU(ID_FILE_SCREENSHOT, ModelViewer::OnSave)
	EVT_MENU(ID_FILE_SCREENSHOTCONFIG, ModelViewer::OnSave)
	EVT_MENU(ID_FILE_EXPORTGIF, ModelViewer::OnSave)
	EVT_MENU(ID_FILE_EXPORTAVI, ModelViewer::OnSave)
	// --
	//EVT_MENU(ID_FILE_TEXIMPORT, ModelViewer::OnTex)
	//EVT_MENU(ID_FILE_TEXEXPORT, ModelViewer::OnTex)
	EVT_MENU(ID_FILE_MODELEXPORT, ModelViewer::OnExport)
	// --
	EVT_MENU(ID_FILE_RESETLAYOUT, ModelViewer::OnToggleCommand)
	// --
	EVT_MENU(ID_FILE_EXIT, ModelViewer::OnExit)

	// view menu
	EVT_MENU(ID_SHOW_FILE_LIST, ModelViewer::OnToggleDock)
	EVT_MENU(ID_SHOW_ANIM, ModelViewer::OnToggleDock)
	EVT_MENU(ID_SHOW_CHAR, ModelViewer::OnToggleDock)
	EVT_MENU(ID_SHOW_LIGHT, ModelViewer::OnToggleDock)
	EVT_MENU(ID_SHOW_MODEL, ModelViewer::OnToggleDock)
	EVT_MENU(ID_SHOW_MODELBANK, ModelViewer::OnToggleDock)	
	// --
	EVT_MENU(ID_SHOW_MASK, ModelViewer::OnToggleCommand)
	//EVT_MENU(ID_SHOW_WIREFRAME, ModelViewer::OnToggleCommand)
	//EVT_MENU(ID_SHOW_BONES, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_SHOW_BOUNDS, ModelViewer::OnToggleCommand)
	//EVT_MENU(ID_SHOW_PARTICLES, ModelViewer::OnToggleCommand)

	EVT_MENU(ID_BACKGROUND, ModelViewer::OnBackground)
	EVT_MENU(ID_BG_COLOR, ModelViewer::OnSetColor)
	EVT_MENU(ID_SKYBOX, ModelViewer::OnBackground)
	EVT_MENU(ID_SHOW_GRID, ModelViewer::OnToggleCommand)

	EVT_MENU(ID_USE_CAMERA, ModelViewer::OnToggleCommand)

	// Cam
	EVT_MENU(ID_CAM_FRONT, ModelViewer::OnCamMenu)
	EVT_MENU(ID_CAM_SIDE, ModelViewer::OnCamMenu)
	EVT_MENU(ID_CAM_BACK, ModelViewer::OnCamMenu)
	EVT_MENU(ID_CAM_ISO, ModelViewer::OnCamMenu)

	EVT_MENU(ID_CANVAS120, ModelViewer::OnCanvasSize)
	EVT_MENU(ID_CANVAS512, ModelViewer::OnCanvasSize)
	EVT_MENU(ID_CANVAS640, ModelViewer::OnCanvasSize)
	EVT_MENU(ID_CANVAS800, ModelViewer::OnCanvasSize)
	EVT_MENU(ID_CANVAS1024, ModelViewer::OnCanvasSize)
	EVT_MENU(ID_CANVAS1152, ModelViewer::OnCanvasSize)
	EVT_MENU(ID_CANVAS1280, ModelViewer::OnCanvasSize)
	EVT_MENU(ID_CANVAS1600, ModelViewer::OnCanvasSize)

	// hidden hotkeys for zooming
	EVT_MENU(ID_ZOOM_IN, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_ZOOM_OUT, ModelViewer::OnToggleCommand)

	// Light Menu
	EVT_MENU(ID_LT_SAVE, ModelViewer::OnLightMenu)
	EVT_MENU(ID_LT_LOAD, ModelViewer::OnLightMenu)
	//EVT_MENU(ID_LT_COLOR, ModelViewer::OnSetColor)
	EVT_MENU(ID_LT_TRUE, ModelViewer::OnLightMenu)
	EVT_MENU(ID_LT_AMBIENT, ModelViewer::OnLightMenu)
	EVT_MENU(ID_LT_DIRECTIONAL, ModelViewer::OnLightMenu)
	EVT_MENU(ID_LT_MODEL, ModelViewer::OnLightMenu)
	EVT_MENU(ID_LT_DIRECTION, ModelViewer::OnLightMenu)
	
	// Effects
	EVT_MENU(ID_ENCHANTS, ModelViewer::OnEffects)
	EVT_MENU(ID_SPELLS, ModelViewer::OnEffects)
	EVT_MENU(ID_EQCREATURE_R, ModelViewer::OnEffects)
	EVT_MENU(ID_EQCREATURE_L, ModelViewer::OnEffects)
	EVT_MENU(ID_SHADER_DEATH, ModelViewer::OnEffects)
	EVT_MENU(ID_TEST, ModelViewer::OnTest)

	// Options
	EVT_MENU(ID_SAVE_CHAR, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_LOAD_CHAR, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_IMPORT_CHAR, ModelViewer::OnToggleCommand)

	EVT_MENU(ID_USE_NPCSKINS, ModelViewer::OnCharToggle)
	EVT_MENU(ID_DEFAULT_DOODADS, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_USE_ANTIALIAS, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_USE_HWACC, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_USE_ENVMAP, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_SHOW_SETTINGS, ModelViewer::OnToggleDock)

	// char controls:
	EVT_MENU(ID_SAVE_EQUIPMENT, ModelViewer::OnSetEquipment)
	EVT_MENU(ID_LOAD_EQUIPMENT, ModelViewer::OnSetEquipment)
	EVT_MENU(ID_CLEAR_EQUIPMENT, ModelViewer::OnSetEquipment)

	EVT_MENU(ID_LOAD_SET, ModelViewer::OnSetEquipment)
	EVT_MENU(ID_LOAD_START, ModelViewer::OnSetEquipment)

	EVT_MENU(ID_SHOW_UNDERWEAR, ModelViewer::OnCharToggle)
	EVT_MENU(ID_SHOW_EARS, ModelViewer::OnCharToggle)
	EVT_MENU(ID_SHOW_HAIR, ModelViewer::OnCharToggle)
	EVT_MENU(ID_SHOW_FACIALHAIR, ModelViewer::OnCharToggle)
	EVT_MENU(ID_SHOW_FEET, ModelViewer::OnCharToggle)
	EVT_MENU(ID_SHEATHE, ModelViewer::OnCharToggle)

	EVT_MENU(ID_MOUNT_CHARACTER, ModelViewer::OnMount)
	EVT_MENU(ID_CHAR_RANDOMISE, ModelViewer::OnSetEquipment)

	// About menu
	EVT_MENU(ID_CHECKFORUPDATE, ModelViewer::OnCheckForUpdate)
	EVT_MENU(ID_HELP, ModelViewer::OnAbout)
	EVT_MENU(ID_ABOUT, ModelViewer::OnAbout)

	// model tree
	EVT_TREE_SEL_CHANGED(ID_FILELIST, ModelViewer::OnTreeSelect)

	// Hidden menu items
	// Temporary saves
	EVT_MENU(ID_SAVE_TEMP1, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_SAVE_TEMP2, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_SAVE_TEMP3, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_SAVE_TEMP4, ModelViewer::OnToggleCommand)

	// Temp loads
	EVT_MENU(ID_LOAD_TEMP1, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_LOAD_TEMP2, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_LOAD_TEMP3, ModelViewer::OnToggleCommand)
	EVT_MENU(ID_LOAD_TEMP4, ModelViewer::OnToggleCommand)

END_EVENT_TABLE()


ModelViewer::ModelViewer()
{
	// our main class objects
	animControl = NULL;
	canvas = NULL;
	charControl = NULL;
	enchants = NULL;
	lightControl = NULL;
	modelControl = NULL;
	arrowControl = NULL;
	imageControl = NULL;
	settingsControl = NULL;
	modelbankControl = NULL;
	animExporter = NULL;
	fileTree = NULL;

	//wxWidget objects
	menuBar = NULL;
	charMenu = NULL;
	viewMenu = NULL;
	optMenu = NULL;
	lightMenu = NULL;
	

	isModel = false;
	isWMO = false;
	isChar = false;

	//wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU
	// create our main frame
	if (Create(NULL, wxID_ANY, wxString(APP_TITLE _T(" ") APP_VERSION), wxDefaultPosition, wxSize(1024, 768), wxDEFAULT_FRAME_STYLE|WS_CLIPCHILDREN, _T("ModelViewerFrame"))) {
		SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
		SetBackgroundStyle(wxBG_STYLE_CUSTOM);

		InitObjects();  // create our canvas, anim control, character control, etc

		// Show our window
		Show(true);
		// Display the window
		Centre();

		// ------
		// Initialise our main window.
		// Load session settings
		LoadSession();

		// create our menu objects
		InitMenu();

		// GUI and Canvas Stuff
		InitDocking();

		// Are these really needed?
		interfaceManager.Update();
		Refresh();
		Update();

		// load our World of Warcraft mpq archives
		Init();
		InitDatabase();
		// --

		// Error check
		if (!initDB) {
			wxMessageBox(_T("Some DBC files could not be loaded.  These files are vital to being able to render models correctly.\nPlease make sure you are loading the 'Locale-xxxx.MPQ' file.\nFile list has been disabled until you are able to correct this problem."), _("DBC Error"));
			fileTree->Disable();
		}

	} else {
		wxLogMessage(_T("Critical Error: Unable to create the main window for the application."));
		Close(true);
	}
}

void ModelViewer::InitMenu()
{
	wxLogMessage(_T("Initiating File Menu.."));
	
	// MENU
	fileMenu = new wxMenu;
	fileMenu->Append(ID_FILE_NPC, _("View NPC"));
	fileMenu->AppendSeparator();
	fileMenu->Append(ID_FILE_SCREENSHOT, _("Save Screenshot\tF12"));
	fileMenu->Append(ID_FILE_SCREENSHOTCONFIG, _("Save Sized Screenshot\tCTRL+S"));
	fileMenu->Append(ID_FILE_EXPORTGIF, _("Animated Gif"));
	fileMenu->Append(ID_FILE_EXPORTAVI, _("Export AVI"));
	fileMenu->AppendSeparator();
	fileMenu->Append(ID_FILE_TEXIMPORT, _("Import Texture"));
	fileMenu->Enable(ID_FILE_TEXIMPORT, false);
	fileMenu->Append(ID_FILE_TEXEXPORT, _("Export Texture"));
	fileMenu->Enable(ID_FILE_TEXEXPORT, false);
	fileMenu->Append(ID_FILE_MODELEXPORT, _("Export Model"));
	fileMenu->AppendSeparator();
	fileMenu->Append(ID_FILE_RESETLAYOUT, _("Reset Layout"));
	fileMenu->AppendSeparator();
	fileMenu->Append(ID_FILE_EXIT, _("E&xit\tCTRL+X"));

	viewMenu = new wxMenu;
	viewMenu->Append(ID_SHOW_FILE_LIST, _("Show file list"));
	viewMenu->Append(ID_SHOW_ANIM, _("Show animaton control"));
	viewMenu->Append(ID_SHOW_CHAR, _("Show character control"));
	viewMenu->Append(ID_SHOW_LIGHT, _("Show light control"));
	viewMenu->Append(ID_SHOW_MODEL, _("Show model control"));
	viewMenu->Append(ID_SHOW_MODELBANK, _("Show model bank"));
	viewMenu->AppendSeparator();
	if (canvas) {
		viewMenu->Append(ID_BG_COLOR, _("Background Color..."));
		viewMenu->AppendCheckItem(ID_BACKGROUND, _("Load Background\tL"));
		viewMenu->Check(ID_BACKGROUND, canvas->drawBackground);
		viewMenu->AppendCheckItem(ID_SKYBOX, _("Skybox"));
		viewMenu->Check(ID_SKYBOX, canvas->drawSky);
		viewMenu->AppendCheckItem(ID_SHOW_GRID, _("Show Grid"));
		viewMenu->Check(ID_SHOW_GRID, canvas->drawGrid);

		viewMenu->AppendCheckItem(ID_SHOW_MASK, _("Show Mask"));
		viewMenu->Check(ID_SHOW_MASK, false);

		viewMenu->AppendSeparator();
	}
	
	

	try {
		
		// Camera Menu
		wxMenu *camMenu = new wxMenu;
		camMenu->AppendCheckItem(ID_USE_CAMERA, _("Use model camera"));
		camMenu->AppendSeparator();
		camMenu->Append(ID_CAM_FRONT, _("Front"));
		camMenu->Append(ID_CAM_BACK, _("Back"));
		camMenu->Append(ID_CAM_SIDE, _("Side"));
		camMenu->Append(ID_CAM_ISO, _("Perspective"));

		viewMenu->Append(ID_CAMERA, _("Camera"), camMenu);
		viewMenu->AppendSeparator();

		wxMenu *setSize = new wxMenu;
		setSize->AppendRadioItem(ID_CANVAS120, _T("120 x 120"));
		setSize->AppendRadioItem(ID_CANVAS512, _T("512 x 512"));
		setSize->AppendRadioItem(ID_CANVAS640, _T("640 x 480"));
		setSize->AppendRadioItem(ID_CANVAS800, _T("800 x 600"));
		setSize->AppendRadioItem(ID_CANVAS1024, _T("1024 x 768"));
		setSize->AppendRadioItem(ID_CANVAS1152, _T("1152 x 864"));
		setSize->AppendRadioItem(ID_CANVAS1280, _T("1280 x 768"));
		setSize->AppendRadioItem(ID_CANVAS1600, _T("1600 x 1200"));

		viewMenu->Append(ID_CANVASSIZE, _("Set Canvas Size"), setSize);
		
		//lightMenu->Append(ID_LT_COLOR, _("Lighting Color..."));

		lightMenu = new wxMenu;
		lightMenu->Append(ID_LT_SAVE, _("Save Lighting"));
		lightMenu->Append(ID_LT_LOAD, _("Load Lighting"));
		lightMenu->AppendSeparator();
		lightMenu->AppendCheckItem(ID_LT_DIRECTION, _("Render Light Objects"));
		lightMenu->AppendSeparator();
		lightMenu->AppendCheckItem(ID_LT_TRUE, _("Use true lighting"));
		lightMenu->Check(ID_LT_TRUE, false);
		lightMenu->AppendRadioItem(ID_LT_DIRECTIONAL, _("Use dynamic light"));
		lightMenu->Check(ID_LT_DIRECTIONAL, true);
		lightMenu->AppendRadioItem(ID_LT_AMBIENT, _("Use ambient light"));
		lightMenu->AppendRadioItem(ID_LT_MODEL, _("Model lights only"));

		charMenu = new wxMenu;
		charMenu->AppendCheckItem(ID_SHOW_UNDERWEAR, _("Show Underwear"));
		charMenu->Check(ID_SHOW_UNDERWEAR, true);
		charMenu->AppendCheckItem(ID_SHOW_EARS, _("Show Ears\tE"));
		charMenu->Check(ID_SHOW_EARS, true);
		charMenu->AppendCheckItem(ID_SHOW_HAIR, _("Show Hair\tH"));
		charMenu->Check(ID_SHOW_HAIR, true);
		charMenu->AppendCheckItem(ID_SHOW_FACIALHAIR, _("Show Facial Hair\tF"));
		charMenu->Check(ID_SHOW_FACIALHAIR, true);
		charMenu->AppendCheckItem(ID_SHOW_FEET, _("Show Feet"));
		charMenu->Check(ID_SHOW_FEET, false);
		charMenu->AppendCheckItem(ID_SHEATHE, _("Sheathe Weapons\tS"));
		charMenu->Check(ID_SHEATHE, false);
		charMenu->AppendSeparator();
		charMenu->Append(ID_SAVE_EQUIPMENT, _("Save Equipment\tF5"));
		charMenu->Append(ID_LOAD_EQUIPMENT, _("Load Equipment\tF6"));
		charMenu->Append(ID_CLEAR_EQUIPMENT, _("Clear Equipment\tF9"));
		charMenu->AppendSeparator();
		charMenu->Append(ID_LOAD_SET, _("Load Item Set"));
		charMenu->Append(ID_LOAD_START, _("Load Start Outfit"));
		charMenu->AppendSeparator();
		charMenu->Append(ID_MOUNT_CHARACTER, _("Mount a character..."));
		charMenu->Append(ID_CHAR_RANDOMISE, _("Randomise Character\tF10"));

		wxMenu *effectsMenu = new wxMenu;
		effectsMenu->Append(ID_ENCHANTS, _("Apply Enchants"));
		effectsMenu->Append(ID_SPELLS, _("Spell Effects"));
		effectsMenu->Enable(ID_SPELLS, false);
		effectsMenu->Append(ID_EQCREATURE_R, _("Creature Right-Hand"));
		effectsMenu->Append(ID_EQCREATURE_L, _("Creature Left-Hand"));
		effectsMenu->Append(ID_SHADER_DEATH, _("Death Effect"));
		effectsMenu->Enable(ID_SHADER_DEATH, false);
		effectsMenu->Append(ID_TEST, _T("TEST"));

		// Options menu
		optMenu = new wxMenu;
		optMenu->Append(ID_SAVE_CHAR, _("Save Character\tF7"));
		optMenu->Append(ID_LOAD_CHAR, _("Load Character\tF8"));
		optMenu->Append(ID_IMPORT_CHAR, _("Import Armory Character"));
		optMenu->AppendSeparator();
		optMenu->AppendCheckItem(ID_USE_NPCSKINS, _("Use npc character skins"));
		optMenu->Check(ID_USE_NPCSKINS, false);
		optMenu->AppendCheckItem(ID_DEFAULT_DOODADS, _T("Always show default doodads in WMOs"));
		optMenu->Check(ID_DEFAULT_DOODADS, true);
		optMenu->AppendSeparator();
		optMenu->Append(ID_SHOW_SETTINGS, _("Settings"));


		wxMenu *aboutMenu = new wxMenu;
		aboutMenu->Append(ID_HELP, _("Help"));
		aboutMenu->Enable(ID_HELP, false);
		aboutMenu->Append(ID_ABOUT, _("About"));
		aboutMenu->AppendSeparator();
		aboutMenu->Append(ID_CHECKFORUPDATE, _("Check for Update"));

		menuBar = new wxMenuBar();
		menuBar->Append(fileMenu, _("&File"));
		menuBar->Append(viewMenu, _("&View"));
		menuBar->Append(charMenu, _("&Character"));
		menuBar->Append(lightMenu, _("&Lighting"));
		menuBar->Append(optMenu, _("&Options"));
		menuBar->Append(effectsMenu, _("&Effects"));
		menuBar->Append(aboutMenu, _("&About"));
		SetMenuBar(menuBar);
	} catch(...) {};

	// Disable our "Character" menu, only accessible when a character model is being displayed
	menuBar->EnableTop(2, false);
	
	// Hotkeys / shortcuts
	wxAcceleratorEntry entries[26];
	entries[0].Set(wxACCEL_NORMAL,  WXK_F5,     ID_SAVE_EQUIPMENT);
	entries[1].Set(wxACCEL_NORMAL,  WXK_F6,     ID_LOAD_EQUIPMENT);
	entries[2].Set(wxACCEL_NORMAL,  WXK_F7,     ID_SAVE_CHAR);
	entries[3].Set(wxACCEL_NORMAL,	WXK_F8,     ID_LOAD_CHAR);
	entries[4].Set(wxACCEL_NORMAL,	(int)'b',	ID_SHOW_BOUNDS);
	//entries[5].Set(wxACCEL_NORMAL,	(int)'B',	ID_SHOW_BOUNDS);
	entries[6].Set(wxACCEL_CTRL,	(int)'X',	ID_FILE_EXIT);
	entries[7].Set(wxACCEL_NORMAL,	WXK_F12,	ID_FILE_SCREENSHOT);
	entries[8].Set(wxACCEL_NORMAL,	(int)'e',	ID_SHOW_EARS);
	entries[9].Set(wxACCEL_NORMAL,	(int)'h',	ID_SHOW_HAIR);
	entries[10].Set(wxACCEL_NORMAL, (int)'f',	ID_SHOW_FACIALHAIR);
	entries[11].Set(wxACCEL_NORMAL, (int)'s',	ID_SHEATHE);
	entries[12].Set(wxACCEL_NORMAL, (int)'l',	ID_BACKGROUND);
	entries[13].Set(wxACCEL_CTRL, (int)'+',		ID_ZOOM_IN);
	entries[14].Set(wxACCEL_CTRL, (int)'-',		ID_ZOOM_OUT);
	entries[15].Set(wxACCEL_CTRL, (int)'s',		ID_FILE_SCREENSHOTCONFIG);
	entries[16].Set(wxACCEL_NORMAL, WXK_F9,		ID_CLEAR_EQUIPMENT);
	entries[17].Set(wxACCEL_NORMAL, WXK_F10,	ID_CHAR_RANDOMISE);

	// Temporary saves
	entries[18].Set(wxACCEL_NORMAL, WXK_F1,		ID_SAVE_TEMP1);
	entries[19].Set(wxACCEL_NORMAL, WXK_F2,		ID_SAVE_TEMP2);
	entries[20].Set(wxACCEL_NORMAL, WXK_F3,		ID_SAVE_TEMP3);
	entries[21].Set(wxACCEL_NORMAL, WXK_F4,		ID_SAVE_TEMP4);

	// Temp loads
	entries[22].Set(wxACCEL_CTRL,	WXK_F1,		ID_LOAD_TEMP1);
	entries[23].Set(wxACCEL_CTRL,	WXK_F2,		ID_LOAD_TEMP2);
	entries[24].Set(wxACCEL_CTRL,	WXK_F3,		ID_LOAD_TEMP3);
	entries[25].Set(wxACCEL_CTRL,	WXK_F4,		ID_LOAD_TEMP4);

	wxAcceleratorTable accel(26, entries);
	this->SetAcceleratorTable(accel);
}

void ModelViewer::InitObjects()
{
	wxLogMessage(_T("Initiating Objects..."));

	fileTree = new wxTreeCtrl(this, ID_FILELIST, wxDefaultPosition, wxSize(100,700), wxTR_HIDE_ROOT|wxTR_HAS_BUTTONS|wxTR_LINES_AT_ROOT);
	fileTree->AddRoot(_T("Root"));

	animControl = new AnimControl(this, ID_ANIM_FRAME);
	charControl = new CharControl(this, ID_CHAR_FRAME);
	lightControl = new LightControl(this, ID_LIGHT_FRAME);
	modelControl = new ModelControl(this, ID_MODEL_FRAME);
	settingsControl = new SettingsControl(this, ID_SETTINGS_FRAME);
	settingsControl->Show(false);
	modelbankControl = new ModelBankControl(this, ID_MODELBANK_FRAME);

	canvas = new ModelCanvas(this);

	if (video.secondPass) {
		canvas->Destroy();
		video.Release();
		canvas = new ModelCanvas(this);
	}

	g_modelViewer = this;
	g_animControl = animControl;
	g_charControl = charControl;
	g_canvas = canvas;

	modelControl->animControl = animControl;
	
	enchants = new EnchantsDialog(this, charControl);

	animExporter = new CAnimationExporter(this, wxID_ANY, _T("Animation Exporter"), wxDefaultPosition, wxSize(350, 220), wxCAPTION|wxSTAY_ON_TOP|wxFRAME_NO_TASKBAR);
}

void ModelViewer::InitDatabase()
{
	wxLogMessage(_T("Initiating Databases..."));
	initDB = true;

	if (!animdb.open()) {
		initDB = false;
		wxLogMessage(_T("Error: Could not open the Animation DB."));
	}
	if (!modeldb.open()) {
		initDB = false;
		wxLogMessage(_T("Error: Could not open the Creatures DB."));
	}
	if (!skindb.open()) {
		initDB = false;
		wxLogMessage(_T("Error: Could not open the CreatureDisplayInfo DB."));
	}
	if(!hairdb.open()) {
		initDB = false;
		wxLogMessage(_T("Error: Could not open the Hair Geoset DB."));
	}
	if(!chardb.open()) {
		initDB = false;
		wxLogMessage(_T("Error: Could not open the Character DB."));
	}
	if(!racedb.open()) {
		initDB = false;
		wxLogMessage(_T("Error: Could not open the Char Races DB."));
	}
	if(!classdb.open()) {
		initDB = false;
		wxLogMessage(_T("Error: Could not open the Char Classes DB."));
	}
	if(!facialhairdb.open()) {
		initDB = false;
		wxLogMessage(_T("Error: Could not open the Char Facial Hair DB."));
	}
	if(!visualdb.open())
		wxLogMessage(_T("Error: Could not open the ItemVisuals DB."));
	if(!effectdb.open())
		wxLogMessage(_T("Error: Could not open the ItemVisualEffects DB."));
	if(!subclassdb.open())
		wxLogMessage(_T("Error: Could not open the Item Subclasses DB."));
	if(!startdb.open())
		wxLogMessage(_T("Error: Could not open the Start Outfit Sets DB."));
	//if(!helmetdb.open()) return false;
	if(!npcdb.open()) 
		wxLogMessage(_T("Error: Could not open the Start Outfit NPC DB."));
	if(!npctypedb.open())
		wxLogMessage(_T("Error: Could not open the Creature Type DB."));

	if(!itemdb.open())
		wxLogMessage(_T("Error: Could not open the ItemDisplayInfo DB."));
	else
		items.cleanup(itemdb);

	if(!setsdb.open())
		wxLogMessage(_T("Error: Could not open the Item Sets DB."));
	else
		setsdb.cleanup(items);

	char filename[20];
	if (langID == 5)
		strncpy(filename, "zhTW/npcs.csv", sizeof(filename));
	else if (langID == 4)
		strncpy(filename, "zhCN/npcs.csv", sizeof(filename));
	else
		strncpy(filename, "enUS/npcs.csv", sizeof(filename));

	if(wxFile::Exists(filename))
		npcs.open(filename);
	else
		wxLogMessage(_T("Error: Could not find npcs.csv, unable to create NPC list."));

	if(spelleffectsdb.open())
		GetSpellEffects();
	else
		wxLogMessage(_T("Error: Could not open the SpellVisualEffects DB."));

	wxLogMessage(_T("Finished initiating database files.\n"));
}

void ModelViewer::InitDocking()
{
	wxLogMessage(_T("Initiating GUI Docking."));
	
	// wxAUI stuff
	//interfaceManager.SetFrame(this); 
	interfaceManager.SetManagedWindow(this);

	// OpenGL Canvas
	interfaceManager.AddPane(canvas, wxAuiPaneInfo().
				Name(wxT("canvas")).Caption(_("OpenGL Canvas")).
				CenterPane());
	
	// Tree list control
	interfaceManager.AddPane(fileTree, wxAuiPaneInfo().
				Name(wxT("fileTree")).Caption(_("File List")).
				BestSize(wxSize(170,700)).Left().Layer(2));

	// Animation frame
    interfaceManager.AddPane(animControl, wxAuiPaneInfo().
				Name(wxT("animControl")).Caption(_("Animation")).
				Bottom().Layer(1));

	// Character frame
	interfaceManager.AddPane(charControl, wxAuiPaneInfo().
                Name(wxT("charControl")).Caption(_("Character")).
                BestSize(wxSize(170,700)).Right().Layer(2).Show(isChar));

	// Lighting control
	interfaceManager.AddPane(lightControl, wxAuiPaneInfo().
		Name(wxT("Lighting")).Caption(_("Lighting")).
		FloatingSize(wxSize(170,430)).Float().Fixed().Show(false).
		DestroyOnClose(false)); //.FloatingPosition(GetStartPosition())

	// model control
	interfaceManager.AddPane(modelControl, wxAuiPaneInfo().
		Name(wxT("Models")).Caption(_("Models")).
		FloatingSize(wxSize(160,460)).Float().Show(false).
		DestroyOnClose(false));

	// model bank control
	interfaceManager.AddPane(modelbankControl, wxAuiPaneInfo().
		Name(_T("ModelBank")).Caption(_("ModelBank")).
		FloatingSize(wxSize(300,320)).Float().Fixed().Show(false).
		DestroyOnClose(false));

	// settings frame
	interfaceManager.AddPane(settingsControl, wxAuiPaneInfo().
		Name(wxT("Settings")).Caption(_("Settings")).
		FloatingSize(wxSize(400,440)).Float().TopDockable(false).LeftDockable(false).
		RightDockable(false).BottomDockable(false).Fixed().Show(false));
                              
    // tell the manager to "commit" all the changes just made
    //interfaceManager.Update();
}

void ModelViewer::ResetLayout()
{
	interfaceManager.DetachPane(fileTree);
	interfaceManager.DetachPane(animControl);
	interfaceManager.DetachPane(charControl);
	interfaceManager.DetachPane(lightControl);
	interfaceManager.DetachPane(modelControl);
	interfaceManager.DetachPane(settingsControl);
	interfaceManager.DetachPane(canvas);
	
	// OpenGL Canvas
	interfaceManager.AddPane(canvas, wxAuiPaneInfo().
				Name(wxT("canvas")).Caption(_("OpenGL Canvas")).
				CenterPane());
	
	// Tree list control
	interfaceManager.AddPane(fileTree, wxAuiPaneInfo().
				Name(wxT("fileTree")).Caption(_("File List")).
				BestSize(wxSize(170,700)).Left().Layer(2));

	// Animation frame
    interfaceManager.AddPane(animControl, wxAuiPaneInfo().
				Name(wxT("animControl")).Caption(_("Animation")).
				Bottom().Layer(1));

	// Character frame
	interfaceManager.AddPane(charControl, wxAuiPaneInfo().
                Name(wxT("charControl")).Caption(_("Character")).
                BestSize(wxSize(170,700)).Right().Layer(2).Show(isChar));

	interfaceManager.AddPane(lightControl, wxAuiPaneInfo().
		Name(wxT("Lighting")).Caption(_("Lighting")).
		FloatingSize(wxSize(170,430)).Float().Fixed().Show(false).
		DestroyOnClose(false)); //.FloatingPosition(GetStartPosition())

	interfaceManager.AddPane(modelControl, wxAuiPaneInfo().
		Name(wxT("Models")).Caption(_("Models")).
		FloatingSize(wxSize(160,460)).Float().Show(false).
		DestroyOnClose(false));

	interfaceManager.AddPane(settingsControl, wxAuiPaneInfo().
		Name(wxT("Settings")).Caption(_("Settings")).
		FloatingSize(wxSize(400,440)).Float().TopDockable(false).LeftDockable(false).
		RightDockable(false).BottomDockable(false).Show(false));

    // tell the manager to "commit" all the changes just made
    interfaceManager.Update();
}


void ModelViewer::LoadSession()
{
	wxLogMessage(_T("Loading Session settings from: %s\n"), cfgPath);

	// Application Config Settings
	wxFileConfig *pConfig = new wxFileConfig(_T("Global"),wxEmptyString, cfgPath, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);

	// Other session settings
	if (canvas) {
		pConfig->SetPath(_T("/Session"));
		double c;
		// Background Colour
		pConfig->Read(_T("bgR"), &c, 0.1f);
		canvas->vecBGColor.x = c;
		pConfig->Read(_T("bgG"), &c, 0.2f);
		canvas->vecBGColor.y = c;
		pConfig->Read(_T("bgB"), &c, 0.4f);
		canvas->vecBGColor.z = c;
		
		// boolean vars
		pConfig->Read(_T("RandomLooks"), &useRandomLooks, true);
		pConfig->Read(_T("DBackground"), &canvas->drawBackground, false);
		pConfig->Read(_T("BackgroundImage"), &bgImagePath, "");
		if (bgImagePath != "") {
			canvas->LoadBackground(bgImagePath);
			//viewMenu->Check(ID_BACKGROUND, canvas->drawBackground);
		}
		

		// model file
		/*wxString modelfn;
		pConfig->Read(_T("Model"), &modelfn);
		if (modelfn) {
			LoadModel(modelfn);
		}*/
	}

	wxDELETE(pConfig);
}

void ModelViewer::SaveSession()
{
	// Application Config Settings
	wxFileConfig *pConfig = new wxFileConfig(_T("Global"), wxEmptyString, cfgPath, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);


	// Graphic / Video display settings
	pConfig->SetPath(_T("/Graphics"));
	pConfig->Write(_T("FSAA"), video.curCap.aaSamples);
	pConfig->Write(_T("AccumulationBuffer"), video.curCap.accum);
	pConfig->Write(_T("AlphaBits"), video.curCap.alpha);
	pConfig->Write(_T("ColourBits"), video.curCap.colour);
	pConfig->Write(_T("DoubleBuffer"), video.curCap.doubleBuffer);
	pConfig->Write(_T("HWAcceleration"), video.curCap.hwAcc);
	pConfig->Write(_T("SampleBuffer"), video.curCap.sampleBuffer);
	pConfig->Write(_T("StencilBuffer"), video.curCap.stencil);
	pConfig->Write(_T("ZBuffer"), video.curCap.zBuffer);
	


	pConfig->SetPath(_T("/Session"));
	// Attempt at saving colour values as 3 byte hex - loss of accuracy from float
	//wxString temp(Vec3DToString(canvas->vecBGColor));

	if (canvas) {
		pConfig->Write(_T("bgR"), (double)canvas->vecBGColor.x);
		pConfig->Write(_T("bgG"), (double)canvas->vecBGColor.y);
		pConfig->Write(_T("bgB"), (double)canvas->vecBGColor.z);
		
		// boolean vars
		pConfig->Write(_T("RandomLooks"), useRandomLooks);

		pConfig->Write(_T("DBackground"), canvas->drawBackground);
		if (canvas->drawBackground)
			pConfig->Write(_T("BackgroundImage"), bgImagePath);
		else
			pConfig->Write(_T("BackgroundImage"), "");

		// model file
		if (canvas->model)
			pConfig->Write(_T("Model"), canvas->model->name);
	}

	// character details
	// equipment

	wxDELETE(pConfig);
}

void ModelViewer::LoadLayout()
{
	// Application Config Settings
	wxFileConfig *pConfig = new wxFileConfig(_T("Global"), wxEmptyString, cfgPath, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);

	wxString layout;

	// Get layout data
	pConfig->SetPath(_T("/Session"));
	pConfig->Read(_T("Layout"), &layout);

	// if the layout data exists,  load it.
	if (!layout.IsNull() && !layout.IsEmpty()) {
		if (!interfaceManager.LoadPerspective(layout, false))
			wxLogMessage(_T("Error: Could not load the layout."));
		else {
			// No need to display these windows on startup
			interfaceManager.GetPane(modelControl).Show(false);
			interfaceManager.GetPane(settingsControl).Show(false);

			// If character panel is showing,  hide it
			interfaceManager.GetPane(charControl).Show(isChar);

			interfaceManager.Update();

			wxLogMessage(_T("Info: GUI Layout loaded from previous session."));
		}
	}

	wxDELETE(pConfig);
}

void ModelViewer::SaveLayout()
{
	// Application Config Settings
	wxFileConfig *pConfig = new wxFileConfig(_T("Global"), wxEmptyString, cfgPath, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);

	pConfig->SetPath(_T("/Session"));
	
	// Save GUI layout data
	wxString layout = interfaceManager.SavePerspective();
	pConfig->Write(_T("Layout"), layout);

	wxLogMessage(_T("Info: GUI Layout was saved."));

	wxDELETE(pConfig);
}


void ModelViewer::LoadModel(const wxString fn)
{
	if (!canvas || fn.IsEmpty())
		return;

	isModel = true;

	// check if this is a character model
	isChar = (fn.Mid(0,4).IsSameAs(_T("Char"), false));

	Attachment *modelAtt = NULL;

	if (isChar) {
		modelAtt = canvas->LoadCharModel(fn.fn_str());

		// error check
		if (!modelAtt) {
			wxLogMessage(_T("Error: Failed to load the model - %s"), fn.c_str());
			return;
		}

		canvas->model->modelType = MT_CHAR;

	} else {
		modelAtt = canvas->LoadModel(fn.fn_str());

		// error check
		if (!modelAtt) {
			wxLogMessage(_T("Error: Failed to load the model - %s"), fn.c_str());
			return;
		}

		canvas->model->modelType = MT_NORMAL;
	}

	// Error check,  make sure the model was actually loaded and set to canvas->model
	if (!canvas->model) {
		wxLogMessage("Error: [ModelViewer::LoadModel()]  Model* Canvas::model is null!");
		return;
	}

	canvas->model->charModelDetails.isChar = isChar;
	
	viewMenu->Enable(ID_USE_CAMERA, canvas->model->hasCamera);
	if (canvas->useCamera && !canvas->model->hasCamera) {
		canvas->useCamera = false;
		viewMenu->Check(ID_USE_CAMERA, false);
	}
	
	// wxAUI
	interfaceManager.GetPane(charControl).Show(isChar);
	if (isChar) {
		charMenu->Check(ID_SHOW_UNDERWEAR, true);
		charMenu->Check(ID_SHOW_EARS, true);
		charMenu->Check(ID_SHOW_HAIR, true);
		charMenu->Check(ID_SHOW_FACIALHAIR, true);

		charControl->UpdateModel(modelAtt);
	}

	// (Dis/en)able Character menu
	menuBar->EnableTop(2, isChar);

	// Update the model control
	modelControl->UpdateModel(modelAtt);
	modelControl->RefreshModel(canvas->root);

	// Update the animations / skins
	animControl->UpdateModel(canvas->model);

	interfaceManager.Update();
}

// Load an NPC model
void ModelViewer::LoadNPC(unsigned int modelid)
{
	canvas->clearAttachments();
	if (!isChar)
		wxDELETE(canvas->model);
	canvas->model = NULL;
	
	isModel = true;
	isChar = false;
	isWMO = false;

	try {
		CreatureSkinDB::Record modelRec = skindb.getBySkinID(modelid);
		int displayID = modelRec.getUInt(CreatureSkinDB::NPCID);

		// if the creature ID ISN'T a "NPC",  then load the creature model and skin it.
		if (displayID == 0) {
			
			unsigned int modelID = modelRec.getUInt(CreatureSkinDB::ModelID);
			CreatureModelDB::Record creatureModelRec = modeldb.getByID(modelID);
			
			wxString name(creatureModelRec.getString(CreatureModelDB::Filename));
			name = name.BeforeLast('.');
			name.Append(_T(".m2"));

			LoadModel(name);
			canvas->model->modelType = MT_NORMAL;

			TextureGroup grp;
			int count = 0;
			for (int i=0; i<TextureGroup::num; i++) {
				//const char *skin = it->getString(CreatureSkinDB::Skin + i);
				std::string skin(modelRec.getString(CreatureSkinDB::Skin + i));
				
				grp.tex[i] = skin;
				if (skin.length() > 0)
					count++;
			}
			grp.base = 11;
			grp.count = count;
			if (grp.tex[0].length() > 0) 
				animControl->AddSkin(grp);

		} else {
			isChar = true;
			NPCDB::Record rec = npcdb.getByNPCID(displayID);
			CharRacesDB::Record rec2 = racedb.getById(rec.getUInt(NPCDB::RaceID));
			
			string retval = rec.getString(NPCDB::Gender);
			string strModel = "Character\\";

			if (retval != "") {
				strModel.append(rec2.getString(CharRacesDB::Name));
				strModel.append("\\Female\\");
				strModel.append(rec2.getString(CharRacesDB::Name));
				strModel.append("Female.m2");
			} else {
				strModel.append(rec2.getString(CharRacesDB::Name));
				strModel.append("\\Male\\");
				strModel.append(rec2.getString(CharRacesDB::Name));
				strModel.append("Male.m2");
			}
			
			//const char *newName = strModel.c_str();

			Attachment *modelAtt;
			modelAtt = canvas->LoadCharModel(strModel.c_str());
			canvas->model->modelType = MT_NPC;

			wxString fn(_T("Textures\\Bakednpctextures\\"));
			fn.Append(rec.getString(NPCDB::Filename));
			charControl->UpdateNPCModel(modelAtt, displayID);
			charControl->customSkin = fn;

			charControl->RefreshNPCModel(); // rec.getUInt(NPCDB::NPCID
			charControl->RefreshEquipment();

			menuBar->EnableTop(2, true);
			charMenu->Check(ID_SHOW_UNDERWEAR, true);
			charMenu->Check(ID_SHOW_EARS, true);
			charMenu->Check(ID_SHOW_HAIR, true);
			charMenu->Check(ID_SHOW_FACIALHAIR, true);
			// ---

			animControl->UpdateModel(canvas->model);
			canvas->ResetView();
		}
	} catch (...) {}

	// wxAUI
	interfaceManager.GetPane(charControl).Show(isChar);
	interfaceManager.Update();
}

void ModelViewer::LoadItem(unsigned int displayID)
{
	
}

// This is called when the user goes to File->Exit
void ModelViewer::OnExit(wxCommandEvent &event)
{
	if (event.GetId() == ID_FILE_EXIT) {
		video.render = false;
		//canvas->timer.Stop();
		canvas->Disable();
		Close(false);
	}
}

// This is called when the window is closing
void ModelViewer::OnClose(wxCloseEvent &event)
{
	Destroy();
}

// Called when the window is resized, minimised, etc
void ModelViewer::OnSize(wxSizeEvent &event)
{
	/* // wxIFM stuff
	if(!interfaceManager)
		event.Skip();
	else
        interfaceManager->Update(IFM_DEFAULT_RECT);
	*/

	// wxAUI
	//interfaceManager.Update(); // causes an error?
}

ModelViewer::~ModelViewer()
{
	wxLogMessage(_T("Shuting down the program...\n"));

	video.render = false;

	// If we have a canvas (which we always should)
	// Stop rendering, give more power back to the CPU to close this sucker down!
	//if (canvas)
	//	canvas->timer.Stop();

	// Save current layout
	SaveLayout();

	// wxAUI stuff
	interfaceManager.UnInit();

	// Clear the MPQ archives.
	for (std::vector<MPQArchive*>::iterator it = archives.begin(); it != archives.end(); ++it) {
        (*it)->close();
		//archives.erase(it);
	}
	archives.clear();

	// Save our session and layout info
	SaveSession();

	if (animExporter) {
		animExporter->Destroy();
		wxDELETE(animExporter);
	}

	if (canvas) {
		canvas->Disable();
		canvas->Destroy(); 
		canvas = NULL;
	}
	
	if (fileTree) {
		fileTree->Destroy();
		fileTree = NULL;
	}

	if (animControl) {
		animControl->Destroy();
		animControl = NULL;
	}

	if (charControl) {
		charControl->Destroy();
		charControl = NULL;
	}

	if (lightControl) {
		lightControl->Destroy();
		lightControl = NULL;
	}

	if (settingsControl) {
		settingsControl->Destroy();
		settingsControl = NULL;
	}

	if (modelControl) {
		modelControl->Destroy();
		modelControl = NULL;
	}

	if (enchants) {
		enchants->Destroy();
		enchants = NULL;
	}
}

class FileTreeData:public wxTreeItemData
{
public:
	std::string fn;
	FileTreeData(std::string fn):fn(fn) {}
};

void ModelViewer::InitMPQArchives()
{
	wxString path;

	for (size_t i=0; i<mpqArchives.GetCount(); i++) {
		if (wxFileName::FileExists(mpqArchives[i]))
			archives.push_back(new MPQArchive(mpqArchives[i]));
	}

	// Checks and logs the "TOC" version of the interface files that were loaded
	MPQFile f(_T("Interface\\FrameXML\\FrameXML.TOC"));
	if (!f.isEof()) {
		f.seek(51);
		unsigned char toc[6];
		memset(toc,'\0', 6);
		f.read(toc, 6);		
		wxLogMessage(_T("Loaded Content TOC: v%c.%c%c.%c%c\n"), toc[0], toc[1], toc[2], toc[3], toc[4]);
	}
	f.close();
}

void ModelViewer::Init()
{
	/*
	// Set our display mode	
	//if (video.GetCompatibleWinMode(video.curCap)) {
		video.SetMode();
		if (!video.render) // Something bad must of happened - find a new working display mode
			video.GetAvailableMode();
	/*
	} else {
		wxLogMessage(_T("Error: Failed to find a compatible graphics mode.  Finding first available display mode..."));
		video.GetAvailableMode(); // Get first available display mode that supports the current desktop colour bitdepth
	}
	*/
	
	wxLogMessage(_T("Setting OpenGL render state..."));
	video.InitGL();

	// Initiate other stuff
	wxLogMessage(_T("Initiating Archives...\n"));

	// more detail logging, this is so when someone has a problem and they send their log info
	wxLogMessage(_T("Game Data Path: %s"), gamePath.fn_str());
	wxLogMessage(_T("Use Local Files: %s\n"), useLocalFiles ? "true" : "false");
	
	isChar = false;
	isModel = false;

	// Load the games MPQ files into memory
	InitMPQArchives();

	// Gets the list of files that meet the filter criteria
	// and puts them into an array to be processed into out file tree
	getFileLists(filelist, filterModels);
	//getFileLists(filelist, filterNpcs);
	//getFileLists(filelist, filterSounds);

	// Put all the viewable files into our File Tree.
	TreeStack stack;
	TreeStackItem root;
	wxTreeItemId item;
	root.first = fileTree->GetRootItem();
	root.second = "";
	stack.push_back(root);

	size_t index=0;

	for (std::set<FileTreeItem>::iterator it = filelist.begin(); it != filelist.end(); ++it) {
		const std::string &str = (*it).fn;
		size_t p = 0;
		size_t i;

		// find the matching place in the stack
		for (i=1; i<stack.size(); i++) {
			std::string &comp = stack[i].second;
			bool match = true;
			for (unsigned int j=0; j<comp.length(); j++) {
				if (comp[j] != str[p+j]) {
					match = false;
					break;
				}
			}
			if (match) 
				match &= str[p+comp.length()] == '\\';

			if (!match)
				break;
            p += comp.length() + 1;
		}
		// i-1 is the index of the last matching piece in the stack

		// delete the extra parts off the end of the stack
		size_t numtopop = stack.size()-i;
		for (size_t k=0; k<numtopop; k++) {
			stack.pop_back();
		}
		
		// starting at p, append extra folders
		size_t start = p;
		bool colour=false;
		for (; p<str.length(); p++) {
			if (str[p]=='\\') {
				// we've hit a folder, push it onto the stack
				TreeStackItem newItem;
				newItem.second = str.substr(start, p-start);
				start = p+1;
				newItem.first = fileTree->AppendItem(stack[stack.size()-1].first, wxString(newItem.second.c_str(), wxConvUTF8));
				
				//if (colour == true) {
					if ((*it).col == 0)
						fileTree->SetItemTextColour(newItem.first, *wxBLACK);
					else if ((*it).col == 1)
						fileTree->SetItemTextColour(newItem.first, *wxBLUE);
					else if ((*it).col == 2)
						fileTree->SetItemTextColour(newItem.first, *wxRED);
					else if ((*it).col == 3)
						fileTree->SetItemTextColour(newItem.first, wxColour(160,0,160));
				//} else {
				//	colour = true;
				//}
					
				stack.push_back(newItem);
			}
		}
		// now start was at the character after the last \\, so we append a filename
		std::string fileName = str.substr(start);

		item = fileTree->AppendItem(stack[stack.size()-1].first, wxString(fileName.c_str(), *wxConvCurrent), -1, -1, new FileTreeData(str));
		if ((*it).col == 0)
			fileTree->SetItemTextColour(item, *wxBLACK);
		else if ((*it).col == 1)
			fileTree->SetItemTextColour(item, *wxBLUE);
		else if ((*it).col == 2)
			fileTree->SetItemTextColour(item, *wxRED);
		else if ((*it).col == 3)
			fileTree->SetItemTextColour(item, wxColour(160,0,160));

		index++;

	}

	filelist.clear();
	
	charControl->Init();
}

bool filterModels(std::string s)
{
	//s.LowerCase();
	const size_t len = s.length();
	if (len < 4) 
		return false;

	//return ((s[len-2]=='m' && s[len-1]=='2') || (s[len-3]=='w' && s[len-2]=='m'));
	return ( 
			((s[len-2]|0x20)=='m' && s[len-1]=='2') ||
			((s[len-3]|0x20)=='w' && (s[len-2]|0x20)=='m' && (s[len-1]|0x20)=='o' ) 
			);
}

/*
bool filterNpcs(std::string s)
{
	// textures\BakedNpcTextures\*.*
	if (s.length() < 18) 
		return false;
	
	return (s.substr(9, 8) == "BakedNpc");
}
*/

bool filterSounds(std::string s)
{
	const size_t len = s.length();
	if (len < 4) 
		return false;

	return (
			((s[len-3]|0x20)=='w' && (s[len-2]|0x20)=='a' && (s[len-1]|0x20)=='v') ||
			((s[len-3]|0x20)=='m' && (s[len-2]|0x20)=='p' && s[len-1]=='3')
			);
}

void ModelViewer::OnTreeSelect(wxTreeEvent &event)
{
	wxTreeItemId item = event.GetItem();

	if (!item.IsOk() || !canvas) // make sure that a valid Tree Item was actually selected.
		return;

	FileTreeData *data = (FileTreeData*)fileTree->GetItemData(item);

	// make sure the data (file name) is valid
	if (!data)
		return; // isn't valid, exit.

	// Exit, if its the same model thats currently loaded
	if (canvas->model && !canvas->model->name.empty() && canvas->model->name == data->fn)
		return; // clicked on the same model thats currently loaded, no need to load it again - exit

	// Delete any previous models that were loaded.
	if (isWMO) {
		//canvas->clearAttachments();
		wxDELETE(canvas->wmo);
	} else {
		canvas->clearAttachments();

		// If it was a character model, no need to delete canvas->model, 
		//it was just pointing to a model created as an attachment - just set back to NULL instead.
		//canvas->model = NULL;
		
		if (!isChar) { 
			wxDELETE(canvas->model);
		} else{
			charControl->charAtt = NULL;
		}
	}			

#ifdef _DEBUG
	GLenum err=glGetError();
	if (err)
		wxLogMessage(_T("OGL Error: [0x%x] An error occured."), (unsigned int)err);
	wxLogMessage(_T("Clearing textures from previous model..."));
#endif

	// Texture clearing and debugging
	texturemanager.clear();

#ifdef _DEBUG
	err = glGetError();
	if (err)
		wxLogMessage(_T("OpenGL Error: [0x%x] An error occured."), (unsigned int)err);
#endif
	
	wxString rootfn(data->fn);

	// Check to make sure the selected item is a model (an *.m2 file).
	isModel = (rootfn.Last() == '2');
	isChar = false;
	isWMO = false;

	if (isModel) {		
		// not functional yet.
		//if (wxGetKeyState(WXK_SHIFT)) 
		//	canvas->AddModel(rootfn);
		//else
			LoadModel(rootfn);	// Load the model.
	
	} else { // else, it isn't a m2 file, so load the file as a WMO.
		isModel = false;
		isChar = false;
		isWMO = true;

		// is WMO?
		menuBar->EnableTop(2, false);
		//canvas->model->modelType = MT_WMO;

		// if we have selected a non-root wmo, find the root filename
		char dash = rootfn[data->fn.length() - 8];
		char num = rootfn[data->fn.length() - 7];
		bool isroot = !((dash=='_') && (num>='0') && (num<='9'));
		if (!isroot) {
			rootfn.erase(rootfn.length()-8);
			rootfn.append(_T(".wmo"));
		}

		canvas->LoadWMO(rootfn.c_str());
		
		int id = -1;
		if (!isroot) {
			char idnum[4];
			strncpy(idnum, data->fn.c_str() + strlen(data->fn.c_str())-7,3);
			//wxString(data->fn.Substr((data->fn.Length() - 7), 3)).ToLong(&id);
			idnum[3]=0;
			sscanf(idnum,"%d",&id);
		}
		canvas->wmo->loadGroup(id);
		canvas->ResetViewWMO(id);
		animControl->UpdateWMO(canvas->wmo, id);
		
		// wxAUI
		interfaceManager.GetPane(charControl).Show(false);
	}

	// Update the layout
	interfaceManager.Update();
}

// Menu button press events
void ModelViewer::OnToggleDock(wxCommandEvent &event)
{
	int id = event.GetId();

	// wxAUI Stuff
	if (id==ID_SHOW_FILE_LIST)
		interfaceManager.GetPane(fileTree).Show(true);
	else if (id==ID_SHOW_ANIM)
		interfaceManager.GetPane(animControl).Show(true);
	else if (id==ID_SHOW_CHAR && isChar)
		interfaceManager.GetPane(charControl).Show(true);
	else if (id==ID_SHOW_LIGHT)
		interfaceManager.GetPane(lightControl).Show(true);
	else if (id==ID_SHOW_MODEL)
		interfaceManager.GetPane(modelControl).Show(true);
	else if (id==ID_SHOW_SETTINGS) {
		interfaceManager.GetPane(settingsControl).Show(true);
		settingsControl->Open();
	} else if (id==ID_SHOW_MODELBANK) {
		interfaceManager.GetPane(modelbankControl).Show(true);
	}
	interfaceManager.Update();
}

// Menu button press events
void ModelViewer::OnToggleCommand(wxCommandEvent &event)
{
	int id = event.GetId();

	//switch 
	switch(id) {
	case ID_FILE_RESETLAYOUT:
		ResetLayout();
		break;

	/*
	case ID_USE_ANTIALIAS:
		useAntiAlias = event.IsChecked();
		break;

	case ID_USE_ENVMAP:
		useEnvMapping = event.IsChecked();
		break;

	case ID_USE_HWACC:
		if (event.IsChecked() == true)
			disableHWAcc = false;
		else
			disableHWAcc = true;
		break;
	*/
	case ID_SHOW_MASK:
		video.useMasking = !video.useMasking;

	case ID_SHOW_BOUNDS:
		canvas->model->showBounds = !canvas->model->showBounds;
		break;

	case ID_SHOW_GRID:
		canvas->drawGrid = event.IsChecked();
		break;

	case ID_USE_CAMERA:
		canvas->useCamera = event.IsChecked();
		break;

	case ID_DEFAULT_DOODADS:
		// if we have a model...
		if (canvas->wmo) {
			canvas->wmo->includeDefaultDoodads = event.IsChecked();
			canvas->wmo->updateModels();
		}
		animControl->defaultDoodads = event.IsChecked();
		break;

	case ID_SAVE_CHAR:
		{
			wxFileDialog saveDialog(this, _("Save character"), wxEmptyString, wxEmptyString, _T("Character files (*.chr)|*.chr"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
			if (saveDialog.ShowModal()==wxID_OK) {
				SaveChar(saveDialog.GetPath().c_str());
			}
		}
		break;
	case ID_LOAD_CHAR:
		{
			wxFileDialog loadDialog(this, _("Load character"), wxEmptyString, wxEmptyString, _T("Character files (*.chr)|*.chr"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
			if (loadDialog.ShowModal()==wxID_OK) {
				for (int i=0; i<NUM_CHAR_SLOTS; i++)
					charControl->cd.equipment[i] = 0;
				
				LoadChar(loadDialog.GetPath().c_str());
			}
		}
		break;

	case ID_IMPORT_CHAR:
		{
			wxTextEntryDialog dialog(this, _T("Please paste in the URL to the character you wish to import."), _T("Please enter text"), wxEmptyString, wxOK | wxCANCEL | wxCENTRE, wxDefaultPosition);
			if (dialog.ShowModal() == wxID_OK)
				ImportArmoury(dialog.GetValue());
		}
		break;

	case ID_ZOOM_IN:
		canvas->Zoom(0.5f, false);
		break;

	case ID_ZOOM_OUT:
		canvas->Zoom(-0.5f, false);
		break;

	case ID_SAVE_TEMP1:
		canvas->SaveSceneState(1);
		break;
	case ID_SAVE_TEMP2:
		canvas->SaveSceneState(2);
		break;
	case ID_SAVE_TEMP3:
		canvas->SaveSceneState(3);
		break;
	case ID_SAVE_TEMP4:
		canvas->SaveSceneState(4);
		break;
	case ID_LOAD_TEMP1:
		canvas->LoadSceneState(1);
		break;
	case ID_LOAD_TEMP2:
		canvas->LoadSceneState(2);
		break;
	case ID_LOAD_TEMP3:
		canvas->LoadSceneState(3);
		break;
	case ID_LOAD_TEMP4:
		canvas->LoadSceneState(4);
		break;
	}
}

void ModelViewer::OnLightMenu(wxCommandEvent &event)
{
	int id = event.GetId();

	switch (id) {
		case ID_LT_SAVE:
		{
			wxFileDialog dialog(this, _("Save Lighting"), wxEmptyString, wxEmptyString, _T("Scene Lighting (*.lit)|*.lit"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
			
			if (dialog.ShowModal()==wxID_OK) {
				wxString fn = dialog.GetFilename();
				
				ofstream f(fn.fn_str(), ios_base::out|ios_base::trunc);
				f << lightMenu->IsChecked(ID_LT_DIRECTION) << " " << lightMenu->IsChecked(ID_LT_TRUE) << " " << lightMenu->IsChecked(ID_LT_DIRECTIONAL) << " " << lightMenu->IsChecked(ID_LT_AMBIENT) << " " << lightMenu->IsChecked(ID_LT_MODEL) << endl;
				for (int i=0; i<MAX_LIGHTS; i++) {
					f << lightControl->lights[i].ambience.x << " " << lightControl->lights[i].ambience.y << " " << lightControl->lights[i].ambience.z << " " << lightControl->lights[i].arc << " " << lightControl->lights[i].constant_int << " " << lightControl->lights[i].diffuse.x << " " << lightControl->lights[i].diffuse.y << " " << lightControl->lights[i].diffuse.z << " " << lightControl->lights[i].enabled << " " << lightControl->lights[i].linear_int << " " << lightControl->lights[i].pos.x << " " << lightControl->lights[i].pos.y << " " << lightControl->lights[i].pos.z << " " << lightControl->lights[i].quadradic_int << " " << lightControl->lights[i].relative << " " << lightControl->lights[i].specular.x << " " << lightControl->lights[i].specular.y << " " << lightControl->lights[i].specular.z << " " << lightControl->lights[i].target.x << " " << lightControl->lights[i].target.y << " " << lightControl->lights[i].target.z << " " << lightControl->lights[i].type << endl;
				}
				f.close();
			}

			return;

		} case ID_LT_LOAD: {
			wxFileDialog dialog(this, _("Load Lighting"), wxEmptyString, wxEmptyString, _T("Scene Lighting (*.lit)|*.lit"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
			
			if (dialog.ShowModal()==wxID_OK) {
				wxString fn = dialog.GetFilename();
				ifstream f(fn.fn_str());
				
				bool lightObj, lightTrue, lightDir, lightAmb, lightModel;

				//lightMenu->IsChecked(ID_LT_AMBIENT)
				f >> lightObj >> lightTrue >> lightDir >> lightAmb >> lightModel;

				lightMenu->Check(ID_LT_DIRECTION, lightObj);
				lightMenu->Check(ID_LT_TRUE, lightTrue);
				lightMenu->Check(ID_LT_DIRECTIONAL, lightDir);
				lightMenu->Check(ID_LT_AMBIENT, lightAmb);
				lightMenu->Check(ID_LT_MODEL, lightModel);

				for (int i=0; i<MAX_LIGHTS; i++) {
					f >> lightControl->lights[i].ambience.x >> lightControl->lights[i].ambience.y >> lightControl->lights[i].ambience.z >> lightControl->lights[i].arc >> lightControl->lights[i].constant_int >> lightControl->lights[i].diffuse.x >> lightControl->lights[i].diffuse.y >> lightControl->lights[i].diffuse.z >> lightControl->lights[i].enabled >> lightControl->lights[i].linear_int >> lightControl->lights[i].pos.x >> lightControl->lights[i].pos.y >> lightControl->lights[i].pos.z >> lightControl->lights[i].quadradic_int >> lightControl->lights[i].relative >> lightControl->lights[i].specular.x >> lightControl->lights[i].specular.y >> lightControl->lights[i].specular.z >> lightControl->lights[i].target.x >> lightControl->lights[i].target.y >> lightControl->lights[i].target.z >> lightControl->lights[i].type;
				}
				f.close();

				if (lightObj) 
					canvas->drawLightDir = true;

				if (lightDir) {
					canvas->lightType = LIGHT_DYNAMIC; //LT_DIRECTIONAL;
					
					/*
					if (lightTrue) {
						if (event.IsChecked()){
							// Need to reset all our colour, lighting, material back to 'default'
							//GLfloat b[] = {0.5f, 0.4f, 0.4f, 1.0f};
							//glColor4fv(b);
							glDisable(GL_COLOR_MATERIAL);

							glMaterialfv(GL_FRONT, GL_EMISSION, def_emission);
							
							glMaterialfv(GL_FRONT, GL_AMBIENT, def_ambience);
							//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, def_ambience);
							
							glMaterialfv(GL_FRONT, GL_DIFFUSE, def_diffuse);
							glMaterialfv(GL_FRONT, GL_SPECULAR, def_specular);
						} else {
							glEnable(GL_COLOR_MATERIAL);
						}
					}
					*/
				} else if (lightAmb) {
					//glEnable(GL_COLOR_MATERIAL);
					canvas->lightType = LIGHT_AMBIENT;
				} else if (lightModel) {
					canvas->lightType = LIGHT_MODEL_ONLY;
				}

				lightControl->UpdateGL();
				lightControl->Update();
			}
			
			return;
		}
		/* case ID_USE_LIGHTS:
			canvas->useLights = event.IsChecked();
			return;
		*/
		case ID_LT_DIRECTION:
			canvas->drawLightDir = event.IsChecked();
			return;
		case ID_LT_TRUE:
			if (event.IsChecked()){
				// Need to reset all our colour, lighting, material back to 'default'
				//GLfloat b[] = {0.5f, 0.4f, 0.4f, 1.0f};
				//glColor4fv(b);
				glDisable(GL_COLOR_MATERIAL);

				glMaterialfv(GL_FRONT, GL_EMISSION, def_emission);
				glMaterialfv(GL_FRONT, GL_AMBIENT, def_ambience);
				//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, def_ambience);
				
				glMaterialfv(GL_FRONT, GL_DIFFUSE, def_diffuse);
				glMaterialfv(GL_FRONT, GL_SPECULAR, def_specular);				
			} else {
				glEnable(GL_COLOR_MATERIAL);
				//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Vec4D(0.4f,0.4f,0.4f,1.0f));
			}
			
			lightControl->Update();

			return;

		// Ambient lighting
		case ID_LT_AMBIENT:
			//glEnable(GL_COLOR_MATERIAL);
			canvas->lightType = LIGHT_AMBIENT;
			return;

		// Dynamic lighting
		case ID_LT_DIRECTIONAL:
			//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, def_ambience);
			canvas->lightType = LIGHT_DYNAMIC;
			return;

		// Model's ambient lighting
		case ID_LT_MODEL:
			canvas->lightType = LIGHT_MODEL_ONLY;
			return;
	}
}

void ModelViewer::OnCamMenu(wxCommandEvent &event)
{
	int id = event.GetId();
	if (id==ID_CAM_FRONT)
		canvas->model->rot.y = -90.0f;
	else if (id==ID_CAM_BACK)
		canvas->model->rot.y = 90.0f;
	else if (id==ID_CAM_SIDE)
		canvas->model->rot.y = 0.0f;
	else if (id==ID_CAM_ISO) {
		canvas->model->rot.y = -40.0f;
		canvas->model->rot.x = 20.0f;
	}

	//viewControl->Update();	
}

// Menu button press events
void ModelViewer::OnSetColor(wxCommandEvent &event)
{
	int id = event.GetId();
	if (id==ID_BG_COLOR) {
		canvas->vecBGColor = DoSetColor(canvas->vecBGColor);
		canvas->drawBackground = false;
	//} else if (id==ID_LT_COLOR) {
	//	canvas->ltColor = DoSetColor(canvas->ltColor);
	}
}

// Menu button press events
void ModelViewer::OnEffects(wxCommandEvent &event)
{
	int id = event.GetId();

	if (id == ID_ENCHANTS) {
		// Currently, only support enchanting character weapons
		if (isChar)
			enchants->Display();

	} else if (id == ID_SPELLS) {
		wxSingleChoiceDialog spellsDialog(this, _T("Choose"), _T("Select a Spell Effect"), spelleffects);

		if (spellsDialog.ShowModal() == wxID_OK) {
			// TODO: Finish adding support for spells.
		}

	} else if (id == ID_EQCREATURE_R) { // R for righthand
		Model *m = static_cast<Model*>(canvas->root->model);

		// make sure m is a valid pointer to a model
		if (m) {
			// This is an error check to make sure the creature can be equipped.
			for(int k=0; k<40; k++){
				if (m->attLookup[k] == 10) {
					SelectCreatureItem(10, 0, charControl, canvas);
					break;
				}
			}
		}

	} else if (id == ID_EQCREATURE_L) { // L for lefthand
		Model *m = static_cast<Model*>(canvas->root->model);

		// make sure m is a valid pointer to a model
		if (m) {
			// This is an error check to make sure the creature can be equipped.
			for(int k=0; k<40; k++){
				if (m->attLookup[k] == 11) {
					SelectCreatureItem(11, 0, charControl, canvas);
					break;
				}
			}
		}
	} else if (id==ID_SHADER_DEATH) {
		//Shader test("Shaders\\Pixel\\FFXDeath.bls");
		//test.Load();
	}
}

Vec3D ModelViewer::DoSetColor(const Vec3D &defColor)
{
	wxColourData data;
	wxColour dcol((unsigned char)(defColor.x*255.0f), (unsigned char)(defColor.y*255.0f), (unsigned char)(defColor.z*255.0f));
	data.SetChooseFull(true);
	data.SetColour(dcol);
	   
	wxColourDialog dialog(this, &data);

	if (dialog.ShowModal() == wxID_OK) {
		wxColour col = dialog.GetColourData().GetColour();
		return Vec3D(col.Red()/255.0f, col.Green()/255.0f, col.Blue()/255.0f);
	}
	return defColor;
}

void ModelViewer::OnSetEquipment(wxCommandEvent &event)
{
	if (isChar) 
		charControl->OnButton(event);

	UpdateControls();
}

void ModelViewer::OnCharToggle(wxCommandEvent &event)
{
	if (event.GetId() == ID_FILE_NPC)
		charControl->selectNPC();
	else if (isChar) 
		charControl->OnCheck(event);
	
}

void ModelViewer::OnMount(wxCommandEvent &event)
{
	/*
	const unsigned int mountSlot = 0;

	// check if it's mountable
	if (!canvas->viewingModel) return;
	Model *root = (Model*)canvas->root->model;
	if (!root) return;
	if (root->name.substr(0,8)!="Creature") return;
	bool mountable = (root->header.nAttachLookup > mountSlot) && (root->attLookup[mountSlot]!=-1);
	if (!mountable) return;

	std::string fn = charControl->selectCharModel();
	if (fn.length()==0) return;

	canvas->root->delChildren();
	Attachment *att = canvas->root->addChild(fn.c_str(), mountSlot, -1);

	wxHostInfo hi;
	hi = layoutManager->GetDockHost(wxDEFAULT_RIGHT_HOST);
	if (!charControlDockWindow->IsDocked()) {
		layoutManager->DockWindow(charControlDockWindow, hi);
		charControlDockWindow->Show(TRUE);
	}
	charMenu->Check(ID_SHOW_UNDERWEAR, true);
	charMenu->Check(ID_SHOW_EARS, true);
	charMenu->Check(ID_SHOW_HAIR, true);
	charMenu->Check(ID_SHOW_FACIALHAIR, true);

	Model *m = (Model*)att->model;
	charControl->UpdateModel(att);

	menuBar->EnableTop(2, true);
	isChar = true;

	// find a Mount animation (id = 91, let's hope this doesn't change)
	for (size_t i=0; i<m->header.nAnimations; i++) {
		if (m->anims[i].animID == 91) {
			m->currentAnim = (int)i;
			break;
		}
	}
	*/

	charControl->selectMount();
}

void ModelViewer::OnSave(wxCommandEvent &event)
{
	static wxFileName dir = cfgPath;
		
	if (!canvas || (!canvas->model && !canvas->wmo))
		return;

	if (event.GetId() == ID_FILE_SCREENSHOT) {
		wxString tmp = "screenshot_";
		tmp << ssCounter;
		wxFileDialog dialog(this, _("Save screenshot"), dir.GetPath(wxPATH_GET_VOLUME), tmp, _T("Bitmap Images (*.bmp)|*.bmp|TGA Images (*.tga)|*.tga|JPEG Images (*.jpg)|*.jpg|PNG Images (*.png)|*.png"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
		dialog.SetFilterIndex(imgFormat);

		if (dialog.ShowModal()==wxID_OK) {
			imgFormat = dialog.GetFilterIndex();
			tmp = dialog.GetPath();
			dialog.Show(false);
			canvas->Screenshot(tmp);
			dir.SetPath(tmp);
			ssCounter++;
		}

		//canvas->InitView();

	} else if (event.GetId() == ID_FILE_EXPORTGIF) {
		if (canvas->wmo)
			return;

		if (!canvas->model)
			return;

		if (!video.supportFBO && !video.supportPBO) {
			wxMessageBox(_T("This function is currently disabled for video cards that don't\nsupport the FrameBufferObject or PixelBufferObject OpenGL extensions"), _T("Error"));
			return;
		}
		
		wxFileDialog dialog(this, _T("Save Animated Gif"), dir.GetPath(wxPATH_GET_VOLUME), _T("animation.gif"), _T("Animated GIF (*.gif)|*.gif"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
		
		if (dialog.ShowModal()==wxID_OK) {
			// Save the folder location for next time
			dir.SetPath(dialog.GetPath());

			// Show our exporter window			
			animExporter->Init(dialog.GetPath());
			animExporter->Show(true);
		}

	} else if (event.GetId() == ID_FILE_EXPORTAVI) {
		if (canvas->wmo && !canvas->model)
			return;

		if (!video.supportFBO && !video.supportPBO) {
			wxMessageBox(_T("This function is currently disabled for video cards that don't\nsupport the FrameBufferObject or PixelBufferObject OpenGL extensions"), _T("Error"));
			return;
		}
		
		wxFileDialog dialog(this, _T("Save AVI"), dir.GetPath(wxPATH_GET_VOLUME), _T("animation.avi"), _T("animation (*.avi)|*.avi"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
		
		if (dialog.ShowModal()==wxID_OK) {
			animExporter->CreateAvi(dialog.GetPath());
		}

	} else if (event.GetId() == ID_FILE_SCREENSHOTCONFIG) {
		if (!imageControl) {
			imageControl = new ImageControl(this, ID_IMAGE_FRAME, canvas);

			interfaceManager.AddPane(imageControl, wxAuiPaneInfo().
				Name(wxT("Screenshot")).Caption(_("Screenshot")).
				FloatingSize(wxSize(295,145)).Float().Fixed().
				Dockable(false)); //.FloatingPosition(GetStartPosition())
		}

		imageControl->OnShow(&interfaceManager);
	}
}

void ModelViewer::OnBackground(wxCommandEvent &event)
{
	static wxFileName dir = cfgPath;
	
	int id = event.GetId();

	if (id == ID_BACKGROUND) {
		if (event.IsChecked()) {
			wxFileDialog dialog(this, _("Load Background"), dir.GetPath(wxPATH_GET_VOLUME), wxEmptyString, _("Bitmap Images (*.bmp)|*.bmp|TGA Images (*.tga)|*.tga|Jpeg Images (*.jpg)|*.jpg|PNG Images (*.png)|*.png|AVI Video file(*.avi)|*.avi"));
			if (dialog.ShowModal() == wxID_OK) {
				canvas->LoadBackground(dialog.GetPath());
				dir.SetPath(dialog.GetPath());
				viewMenu->Check(ID_BACKGROUND, canvas->drawBackground);
			} else {
				viewMenu->Check(ID_BACKGROUND, false);
			}
		} else {
			canvas->drawBackground = false;
		}
	} else if (id == ID_SKYBOX) {
		if (canvas->skyModel) {
			wxDELETE(canvas->skyModel);
			canvas->sky->delChildren();
			
		} else {

			// List of skybox models
			const wxString skyboxes[] = {
				"Environments\\Stars\\COT_sky01.m2",
				"Environments\\Stars\\CavernsOfTimeSky.m2",
				"Environments\\Stars\\DeathClouds.m2",
				"Environments\\Stars\\DireMaulSkybox.m2",
				"Environments\\Stars\\Stars.m2",
				"Environments\\Stars\\StratholmeSkybox.m2",
				"Environments\\Stars\\ShadowmoonSkybox.m2",
				"Environments\\Stars\\PortalWorldLegionSky.m2",
				"Environments\\Stars\\NetherstormSkybox.m2",
				"Environments\\Stars\\NagrandSkybox.m2",
				"Environments\\Stars\\HellfireSkybox.m2",
				"Environments\\Stars\\BonewasteSkybox.m2",
				"Environments\\Stars\\BladesEdgeSkybox.m2",
				"Environments\\Stars\\BladesEdgeForestSkybox.m2",
				"Environments\\Stars\\BladesEdgeNightelfSkybox.m2",
				"World\\Outland\\PassiveDoodads\\SkyBox\\OutlandSkyBox.m2"
			};
			
			wxSingleChoiceDialog skyDialog(this, _T("Choose"), _T("Select a Sky Box"), WXSIZEOF(skyboxes), skyboxes);
			if (skyDialog.ShowModal() == wxID_OK && skyDialog.GetStringSelection() != "") {
				canvas->skyModel = new Model(skyDialog.GetStringSelection().c_str(), false);
				canvas->sky->model = canvas->skyModel;
			}
		}
		
		canvas->drawSky = event.IsChecked();
	}
}

void ModelViewer::SaveChar(const char *fn)
{
	ofstream f(fn, ios_base::out|ios_base::trunc);
	f << canvas->model->name << endl;
	f << charControl->cd.race << " " << charControl->cd.gender << endl;
	f << charControl->cd.skinColor << " " << charControl->cd.faceType << " " << charControl->cd.hairColor << " " << charControl->cd.hairStyle << " " << charControl->cd.facialHair << " " << charControl->cd.facialColor << endl;
	for (int i=0; i<NUM_CHAR_SLOTS; i++) {
		f << charControl->cd.equipment[i] << endl;
	}

	// 5976 is the ID value for "Guild Tabard"
	if (charControl->cd.equipment[CS_TABARD] == 5976) {
		f << charControl->td.Background << " " << charControl->td.Border << " " << charControl->td.BorderColor << " " << charControl->td.Icon << " " << charControl->td.IconColor << endl;
	}

	f << endl;
	f.close();
}

void ModelViewer::LoadChar(const char *fn)
{
	std::string modelname;
	ifstream f(fn);
	
	f >> modelname; // model name

	// Clear the existing model
	if (isWMO) {
		//canvas->clearAttachments();
		wxDELETE(canvas->wmo);
		canvas->wmo = NULL;
	} else {
		canvas->clearAttachments();
		if (!isChar)
			wxDELETE(canvas->model);
		canvas->model = NULL;
	}

	// Load the model
	LoadModel(modelname.c_str());
	canvas->model->modelType = MT_CHAR;

	f >> charControl->cd.race >> charControl->cd.gender; // race and gender
	f >> charControl->cd.skinColor >> charControl->cd.faceType >> charControl->cd.hairColor >> charControl->cd.hairStyle >> charControl->cd.facialHair >> charControl->cd.facialColor;

	while (!f.eof()) {
		for (int i=0; i<NUM_CHAR_SLOTS; i++) {
			f >> charControl->cd.equipment[i];
		}
		break;
	}

	// 5976 is the ID value for "Guild Tabard"
	if (charControl->cd.equipment[CS_TABARD] == 5976 && !f.eof()) {
		f >> charControl->td.Background >> charControl->td.Border >> charControl->td.BorderColor >> charControl->td.Icon >> charControl->td.IconColor;
		charControl->td.showCustom = true;
	}

	f.close();

	charControl->RefreshModel();
	charControl->RefreshEquipment();

	// Update interface docking components
	interfaceManager.Update();
}

void ModelViewer::OnLanguage(wxCommandEvent &event)
{
	/*
	if (event.GetId() == ID_LOCALE) {
		static const wxLanguage langIds[] =
		{
			wxLANGUAGE_ENGLISH,
			wxLANGUAGE_KOREAN,
			wxLANGUAGE_FRENCH,
			wxLANGUAGE_GERMAN,
			wxLANGUAGE_CHINESE_SIMPLIFIED,
			wxLANGUAGE_CHINESE_TRADITIONAL,
			wxLANGUAGE_SPANISH,
		};
		
		const wxString langNames[] =
		{
			_T("English"),
			_T("Korean"),
			_T("French"),
			_T("German"),
			_T("Chinese (Simplified)"),
			_T("Chinese (Traditional)"),
			_T("Spanish"),
		};

		// the arrays should be in sync
		wxCOMPILE_TIME_ASSERT(WXSIZEOF(langNames) == WXSIZEOF(langIds), LangArraysMismatch);

		long lng = wxGetSingleChoiceIndex(_T("Please select a language:"), _T("Language"), WXSIZEOF(langNames), langNames);
		
		if (lng != -1) {
			langID = lng;
			wxMessageBox(_T("You will need to reload WoW Model Viewer for changes to take effect."), _T("Language Changed"), wxOK | wxICON_INFORMATION);
		}

	}
	*/
}

void ModelViewer::OnAbout(wxCommandEvent &event)
{
/*
text = new wxStaticText(this, wxID_ANY, _T("Developers:		UfoZ, Darjk\n\n\
Pioneers:		UfoZ,  Linghuye,  nSzAbolcs\n\n\
Translators:	Culhag (French), Die_Backe (Deutsch)\n\n\
Developed Using:\n\
wxWidgets(2.6.3), wxAUI(0.9.2), OpenGL, zlib,\n\
CxImage(5.99c), MPQLib, DDSLib, GLEW(1.3.3)\n\n\
Build Info:\nVersion 0.5 compiled using Visual C++ 7.1 (2003) for\n\
Windows 98\\ME\\2000\\XP on 17th December 2006\n\n\
*/

	wxAboutDialogInfo info;
    info.SetName(_T(APP_TITLE));
    info.SetVersion(_T(APP_VERSION));
	info.AddDeveloper(_T("Ufo_Z"));
	info.AddDeveloper(_T("Darjk"));
	info.AddTranslator(_T("Culhag (French)"));
	info.AddTranslator(_T("Die_Backe (Deutsch)"));

	info.SetWebSite(_T("http://www.wowmodelviewer.org/"));
    info.SetCopyright(
wxString(_T("World of Warcraft(R) is a Registered trademark of\n\
Blizzard Entertainment(R). All game assets and content\n\
is (C)2006 Blizzard Entertainment(R). All rights reserved.")));

	info.SetLicence(_T("WoWmodelview is released under the GNU General Public License."));

	info.SetDescription(_T("WoWmodelview is a 3D model viewer for World of Warcraft.\nIt uses the data files included with the game to display\nthe 3D models from the game: creatures, characters, spell\neffects, objects and so forth.\n\nCredits To: Linghuye,  nSzAbolcs,  Sailesh, Terran and Cryect\nfor their contributions either directly or indirectly."));
	//info.SetArtists();
	//info.SetDocWriters();
	//info.SetIcon();

    wxAboutBox(info);
}

void ModelViewer::OnCheckForUpdate(wxCommandEvent &event)
{
	wxURL url(_T("http://www.wowmodelviewer.org/latest.txt"));
	
	if(url.GetError() == wxURL_NOERR)   {
		wxInputStream *stream = url.GetInputStream();
		
		// here, just for example, I read 1024 bytes. You should read what you need...
		char buffer[1024];
		stream->Read(&buffer, 1024);
		
		// Sort the data
		wxString data(buffer);
		wxString version = data.BeforeFirst(13);
		wxString downloadURL = data.AfterLast(10);

		if (version.IsSameAs(APP_VERSION, false)) {
			wxMessageBox(_("You have the most upto date version."), _("Update Check"));
		} else {
			wxString msg = _T("The most current version is: ");
			msg.Append(version);
			msg.Append(_T("\nWould you like to goto the download page?"));
			int answer = wxMessageBox(msg, _("Update Check"), wxYES_NO, this);
			if (answer == wxYES)
				wxLaunchDefaultBrowser(downloadURL);
		}

		// Create a string from the data that was received... (?)
		//wxString webversion;
		//wxMessageBox(wxString::Format("%s",buffer));

		delete stream;
	}
}

void ModelViewer::OnCanvasSize(wxCommandEvent &event)
{
	int id = event.GetId();
	unsigned int sizex = 0;
	unsigned int sizey = 0;
	
	if (id == ID_CANVAS512) {
		sizex = 512;
		sizey = 512;
	} else if (id == ID_CANVAS640) {
		sizex = 640;
		sizey = 480;
	} else if (id == ID_CANVAS800) {
		sizex = 800;
		sizey = 600;
	} else if (id == ID_CANVAS1024) {
		sizex = 1024;
		sizey = 768;
	} else if (id == ID_CANVAS1152) {
		sizex = 1152;
		sizey = 864;
	} else if (id == ID_CANVAS1280) {
		sizex = 1280;
		sizey = 768;
	} else if (id == ID_CANVAS120) {
		sizex = 120;
		sizey = 120;
	} else if (id == ID_CANVAS1600) {
		sizex = 1600;
		sizey = 1200;
	}

	if (sizex > 0 && sizey > 0) {
		int curx=0, cury=0;
		int difx=0, dify=0;

		canvas->GetClientSize(&curx, &cury);
		difx = sizex - curx;
		dify = sizey - cury;
		// if the window is already large enough,  just shrink the canvas
		//if (difx <= 0 && dify <= 0 && sizex < 800 && sizey < 600) {
		//	canvas->SetClientSize(sizex, sizey);
		//} else {
			GetClientSize(&curx, &cury);
			SetSize((curx + difx), (cury + dify));
		//}
	}
}

void ModelViewer::OnExport(wxCommandEvent &event)
{
	if (event.GetId() == ID_FILE_MODELEXPORT) {
		wxFileDialog dialog(this, _("Export Model"), wxEmptyString, wxEmptyString, _T("Wavefront (*.obj)|*.obj|Lightwave (*.lwo)|*.lwo|Milkshape 3D (*.ms3d)|*.ms3d|3D Studio Max (*.3ds)|*.3ds"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
		if (dialog.ShowModal()==wxID_OK) {
			wxLogMessage(_T("Info: Exporting model to %s..."), dialog.GetPath().fn_str());

			if (dialog.GetFilterIndex() == 0)
				ExportM2toOBJ(canvas->model, dialog.GetPath().fn_str());
			else if (dialog.GetFilterIndex() == 1)
				ExportM2toLWO(canvas->model, dialog.GetPath().fn_str());
			else if (dialog.GetFilterIndex() == 2)
				ExportM2toMS3D(canvas->root, dialog.GetPath().fn_str());
			else if (dialog.GetFilterIndex() == 3)
				//wxMessageBox(_T("Error: This format is not yet fully supported."), _T("Error"));
				ExportM2to3DS(canvas->model, dialog.GetPath().fn_str());
		}
	}
}

void ModelViewer::OnTest(wxCommandEvent &event)
{
		if (!charControl->charAtt)
			return;

		if (arrowControl) {
			arrowControl->Show(true);
		} else {
			arrowControl = new ArrowControl(this, wxID_ANY, wxDefaultPosition, charControl->charAtt);
			/* // wxIFM stuff
			arrowControlPanel = new wxIFMDefaultChildData(arrowControl, IFM_CHILD_GENERIC, wxDefaultPosition, wxSize(140, 300), true, _("Arrow Control"));
			arrowControlPanel->m_orientation = IFM_ORIENTATION_FLOAT;
			interfaceManager->AddChild(arrowControlPanel);
			interfaceManager->Update(IFM_DEFAULT_RECT,true);

			interfaceManager->ShowChild(arrowControl,true,true);
			*/
			// wxAUI
			interfaceManager.AddPane(arrowControl, wxAuiPaneInfo().
			Name(wxT("Arrows")).Caption(_("Arrows")).
			FloatingSize(wxSize(150,300)).Float().Show(true)); //.FloatingPosition(GetStartPosition())
			interfaceManager.Update();
		}
}

void ModelViewer::UpdateControls()
{
	if (!canvas || !canvas->model || !canvas->root)
		return;

	if (canvas->model->modelType == MT_NPC)
		charControl->RefreshNPCModel();
	else if (canvas->model->modelType == MT_CHAR)
		charControl->RefreshModel();
	
	modelControl->RefreshModel(canvas->root);
}

void ModelViewer::ImportArmoury(wxString strURL)
{
	// Format the URL
	wxString strDomain = strURL.BeforeLast(_T('/')); // "armory.worldofwarcraft.com"
	strDomain = strDomain.AfterLast(_T('/'));
	wxString strPage = _T('/') + strURL.AfterLast(_T('/')); // "/character-sheet.xml?r=%s&n=%s"
	//http://armory.wow-europe.com/character-sheet.xml?r=Spinebreaker&n=Nostrum

	// Get the page from the armoury website
	wxHTTP http;

	// set the headers
	http.SetHeader("User-Agent", "Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.1.2) Gecko/20070219 Firefox/2.0.0.2 WoWModelViewer/0.5.08"); 
	http.SetHeader("Accept", "text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5");	http.SetHeader("Accept-Language", "en-us,en;q=0.5");	http.SetHeader("Accept-Charset", "ISO-8859-1,utf-8;q=0.7,*;q=0.7");

	if (http.Connect(strDomain))
	{ 
		// Success
		wxInputStream *stream = http.GetInputStream(strPage); 
		if (!stream->IsOk())
			return;

		// Make sure there was no error retrieving the page
		if(http.GetError() == wxURL_NOERR) {
			wxFileOutputStream output("temp.xml");
            stream->Read(output); 
			output.Close();
			
			wxXmlDocument xmlDoc;
			if (!xmlDoc.Load("temp.xml", _T("UTF-8")))
				return;
			
			// start processing the XML file
			if (xmlDoc.GetRoot()->GetName() != wxT("page"))
				return;

			wxXmlNode *child = xmlDoc.GetRoot()->GetChildren();

			if (!child)
				return;

			child = child->GetChildren();

			while (child) {

				if (child->GetName() == _T("character")) {

					// process text enclosed by <tag1></tag1>
					//wxString content = child->GetNodeContent();

					// process properties of <tag1>
					wxString race = child->GetPropVal(_T("race"), _T("Human"));
					//race = race.MakeLower();
					wxString gender = child->GetPropVal(_T("gender"), _T("Male"));
					//gender = gender.MakeLower();

					wxString strModel = _T("Character\\") + race + _T("\\") + gender + _T("\\") + race + gender + _T(".m2");

					LoadModel(strModel);

					if (!g_canvas->model)
						return;

				} else if (child->GetName() == _T("characterTab")) {
					child = child->GetChildren();

				} else if (child->GetName() == _T("items")) {

					wxXmlNode *itemNode = child->GetChildren();
					while (itemNode) {
						if (itemNode->GetName() == _T("item")) {
							wxString id = itemNode->GetPropVal(_T("id"), _T("0"));
							wxString slot = itemNode->GetPropVal(_T("slot"), _T("0"));
							
							// Item ID
							long itemID;
							id.ToLong(&itemID);

							// Equipment Slot
							long slotID;
							slot.ToLong(&slotID);

							// Manual correction for slot ID values
							if (slotID == 4) // Chest
								slotID = CS_CHEST;
							else if (slotID == 5) // Waist
								slotID = CS_BELT;
							else if (slotID == 7) // Feet
								slotID = CS_BOOTS;
							else if (slotID == 14) // Back
								slotID = CS_CAPE;
							else if (slotID == 15) // Right Hand
								slotID = CS_HAND_RIGHT;
							else if (slotID == 16) // Left Hand
								slotID = CS_HAND_LEFT;

							g_charControl->cd.equipment[slotID] = itemID;
						}

						itemNode = itemNode->GetNext();
					}
					
					// Update the model
					g_charControl->RefreshModel();
					g_charControl->RefreshEquipment();
					break;
				}

				child = child->GetNext();
			}
		}
		delete stream;
	}
}

// --

#include <QProcess>
#include <qdir.h>
#include <qmessagebox.h>
#include "Interface_Viewer.h"
#include "../Engine/version.h"
#include "../Engine/lists.h"

using namespace WMVEngine;
using namespace QsLogging;

// Viewer Interface Window
Interface_Viewer::Interface_Viewer(QWidget *parent) : QMainWindow(parent), ui(new Ui::Interface_Viewer)
{

	QLOG_INFO() << "Interface Viewer DLL test";

	// Setup the UI
    ui->setupUi(this);

	// Set Application Data
	QCoreApplication::setApplicationName(PROGRAMNAME);
	QCoreApplication::setApplicationVersion(MajorBuildVersion);
	QCoreApplication::setOrganizationName(ORGANIZATIONNAME);
	QCoreApplication::setOrganizationDomain(ORGANIZATIONWEBSITE);
	
	// Icons
	icon01Vanilla.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon01-Vanilla"), QSize(), QIcon::Normal, QIcon::Off);
    icon01Vanilla.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon01-Vanilla"), QSize(), QIcon::Normal, QIcon::On);
    icon02TBC.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon02-TBC"), QSize(), QIcon::Normal, QIcon::Off);
    icon02TBC.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon02-TBC"), QSize(), QIcon::Normal, QIcon::On);
    icon03WotLK.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon03-WotLK"), QSize(), QIcon::Normal, QIcon::Off);
	icon03WotLK.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon03-WotLK"), QSize(), QIcon::Normal, QIcon::On);
    icon04Cata.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon04-Cataclysm"), QSize(), QIcon::Normal, QIcon::Off);
	icon04Cata.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon04-Cataclysm"), QSize(), QIcon::Normal, QIcon::On);
	icon05MoP.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon05-MoP"), QSize(), QIcon::Normal, QIcon::Off);
	icon05MoP.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon05-MoP"), QSize(), QIcon::Normal, QIcon::On);
    iconPTR.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon-PTR"), QSize(), QIcon::Normal, QIcon::Off);
	iconPTR.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon-PTR"), QSize(), QIcon::Normal, QIcon::On);
	iconDisabled.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon-Disabled"), QSize(), QIcon::Normal, QIcon::Off);
	iconDisabled.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon-Disabled"), QSize(), QIcon::Normal, QIcon::On);

    // Defaults
	isWoWLoaded = false;
	canReloadWoW = false;
	WoWTypeNext = WOW_NOTLOADED;						// The next World of Warcraft type that will be loaded.
    ViewerInterfaceType = VIEWER_INTERFACETYPE_NONE;	// Full list of viewer interface types in enums.h

	// Set the main Window's Title
	//: Passed arguments: ProgramName (WMV), MajorVersion (v0.8.0.0), BuildVersion (r650), SystemVersion ("Windows 64-bit"), DebugVersion (" Debug" or "")
	setWindowTitle(QString("%1 %2 %3%4").arg(PROGRAMNAME,MAJORVERSION,SYSTEMVERSION,DEBUGVERSION));

    /* -= Groups =- */
    // Eye Glow
    EyeGlowGroup = new QActionGroup(this);
    EyeGlowGroup->addAction(ui->actionGlowNone);
    EyeGlowGroup->addAction(ui->actionGlowDefault);
    EyeGlowGroup->addAction(ui->actionGlowDeathKnight);
    ui->actionGlowDefault->setChecked(true);

    // Canvas Sizes
    CanvasSizeGroup = new QActionGroup(this);
    CanvasSizeGroup->addAction(ui->actionCanvasSize11_0120x0120);
    CanvasSizeGroup->addAction(ui->actionCanvasSize11_0256x0256);
    CanvasSizeGroup->addAction(ui->actionCanvasSize11_0512x0512);
    CanvasSizeGroup->addAction(ui->actionCanvasSize11_1024x1024);
    CanvasSizeGroup->addAction(ui->actionCanvasSize43_0640x0480);
    CanvasSizeGroup->addAction(ui->actionCanvasSize43_0800x0600);
    CanvasSizeGroup->addAction(ui->actionCanvasSize43_1024x0768);
    CanvasSizeGroup->addAction(ui->actionCanvasSize43_1280x1024);
    CanvasSizeGroup->addAction(ui->actionCanvasSize43_1600x1200);
    CanvasSizeGroup->addAction(ui->actionCanvasSize169_0864x0480);
    CanvasSizeGroup->addAction(ui->actionCanvasSize169_1280x0720);
    CanvasSizeGroup->addAction(ui->actionCanvasSize169_1920x1080);
    CanvasSizeGroup->addAction(ui->actionCanvasSizeMisc_1280x0768);
    CanvasSizeGroup->addAction(ui->actionCanvasSizeMisc_1920x1200);
    ui->actionCanvasSize43_1024x0768->setChecked(true);

    // ADT Quality
    LandscapeQualityGroup = new QActionGroup(this);
    LandscapeQualityGroup->addAction(ui->actionADTQualityHigh);
	LandscapeQualityGroup->addAction(ui->actionADTQualityMedium);
    LandscapeQualityGroup->addAction(ui->actionADTQualityLow);
    ui->actionADTQualityHigh->setChecked(true);

    // Light Types
    LightTypeGroup = new QActionGroup(this);
    LightTypeGroup->addAction(ui->actionLightType_Dynamic);
    LightTypeGroup->addAction(ui->actionLightType_Ambient);
    LightTypeGroup->addAction(ui->actionLightType_ModelOnly);
    ui->actionLightType_Dynamic->setChecked(true);

	// Viewer Modes
	ViewerModeGroup = new QActionGroup(this);
	ViewerModeGroup->addAction(ui->actionMode_Viewer);
	ViewerModeGroup->addAction(ui->actionMode_Cinema);
	ui->actionMode_Viewer->setChecked(true);

    /* -= Dynamic Groups =- */
	// WoW Dirs
	WoWDirGroup = new QActionGroup(this);

    // Camera Group
    CameraGroup = new QActionGroup(this);
    // Static Views
    CameraGroup->addAction(ui->actionCam_Perspective);
    CameraGroup->addAction(ui->actionCam_Top);
    CameraGroup->addAction(ui->actionCam_Left);
    CameraGroup->addAction(ui->actionCam_Right);
	CameraGroup->addAction(ui->actionCam_Back);
    CameraGroup->addAction(ui->actionCam_Model);
    CameraGroup->addAction(ui->actionCam_Additional);
    ui->actionCam_Perspective->setChecked(true);

    // Doodad Sets
    DoodadSetGroup = new QActionGroup(this);

	// Connections
	QObject::connect(WoWDirGroup, SIGNAL(triggered(QAction*)), this, SLOT(ChangeLoadButton()));

	// UI Cheats
	ui->rBtn_IsChar->setDisabled(true);
	ui->rBtn_IsADT->setDisabled(true);
	ui->rBtn_IsItem->setDisabled(true);
	ui->rBtn_IsNPC->setDisabled(true);
	ui->rBtn_IsSound->setDisabled(true);
	ui->rBtn_IsTexture->setDisabled(true);
	ui->rBtn_IsWMO->setDisabled(true);
	ui->rBtn_NoModel->setDisabled(true);
}

Interface_Viewer::~Interface_Viewer() {
	delete ui;
}

void Interface_Viewer::init()
{
	//Exporters::Exporters();	// Initialize the Exporters
	//CheckSettings_Main();	// Check Main Program Settings
	//WMVEngine::ReadWoWDirList();		// Read the WoW Directory List
	
	/*
	// Dependant Defaults
	InterfaceMode = sWMVSettings.value("LastInterfaceMode").toUInt();	// Set the default mode to Viewer.
	CurrentDir = WoWDirList.value(sWMVSettings.value("CurrentWoWDir").toString(),st_WoWDir());
	WoWTypeCurrent = CurrentDir.Version;								// The currently loaded World of Warcraft type.
	*/
	/*
	// Used to test the global accessablility of the QsLogger.
	QMessageBox b(QMessageBox::Icon::Information,"Interface Viewer Logger Testing...",QString("Viewer Logging Level: %1").arg(QsLog_logger.loggingLevel()).toUtf8());
	b.exec();
	*/

	QLOG_INFO() << "Init Test!";

	//QLOG_INFO() << "Current WoW Directory:" << sWMVSettings.value("CurrentWoWDir").toString();
	//QLOG_INFO() << "Current WoW Version:" << CurrentDir.Version;

	// Build Status Bars
	createStatusBar();
	
	// Process and Update our Menus
	UpdateViewerMenu();
}

void Interface_Viewer::createStatusBar()
{
	statusBar()->setAutoFillBackground(true);
	statusBar()->setSizeGripEnabled(false);
	statusBar()->showMessage(tr("Ready"));
}

void Interface_Viewer::updateStatusBar(QString newstatus)
{
	statusBar()->showMessage(newstatus);
}

void Interface_Viewer::UpdateViewerMenu(){
	QLOG_INFO() << "Updating Viewer Menu...";
    // Remove Menus
    menuBar()->removeAction(ui->menuCharacter->menuAction());
    menuBar()->removeAction(ui->menuNPC->menuAction());
    menuBar()->removeAction(ui->menuSets->menuAction());
    menuBar()->removeAction(ui->menuLandscape->menuAction());
    menuBar()->removeAction(ui->menuImage->menuAction());
	ui->menuChange_WoW_Directory->clear();

	// Disabled functions, because they ONLY work with a model or file loaded
    ui->menuLighting->setDisabled(true);
    ui->menuCamera->setDisabled(true);
    ui->menuExport_Models->setDisabled(true);
	ui->actionSave_Screenshot->setDisabled(true);
	ui->actionSave_Image_Sequence->setDisabled(true);
	ui->actionExport_Textures->setDisabled(true);
	ui->actionSave_File->setDisabled(true);

	// Disable functions if WoW is not loaded, then end.
	if (isWoWLoaded == false){
		ui->actionLoad_World_of_Wacraft->setDisabled(false);
		ui->actionLoad_Character->setDisabled(true);
		ui->actionImportArmoryCharacter->setDisabled(true);
		ui->actionLoad_NPC->setDisabled(true);
		ui->actionLoadSkybox->setDisabled(true);
		ui->actionModel_Bank->setDisabled(true);
		ui->actionSave_File->setDisabled(true);
		ui->actionDiscover_Items->setDisabled(true);
		ui->actionDiscover_NPCs->setDisabled(true);
	}else{
		// Else if WoW is loaded...
		ui->actionLoad_Character->setDisabled(false);
		ui->actionImportArmoryCharacter->setDisabled(false);
		ui->actionLoad_NPC->setDisabled(false);
		ui->actionLoadSkybox->setDisabled(false);
		ui->actionModel_Bank->setDisabled(false);
		ui->actionDiscover_Items->setDisabled(false);
		ui->actionDiscover_NPCs->setDisabled(false);
		if (canReloadWoW == true)
			ui->actionLoad_World_of_Wacraft->setDisabled(false);
		else
			ui->actionLoad_World_of_Wacraft->setDisabled(true);
	}

	// Broad Strokes
	if (ViewerInterfaceType != VIEWER_INTERFACETYPE_NONE){
		ui->actionSave_File->setDisabled(false);
	}

    // Change Options and Insert Menus based on the File Type
    if (ViewerInterfaceType == VIEWER_INTERFACETYPE_IMAGE){
		ui->actionSave_Screenshot->setDisabled(false);
		ui->actionSave_Image_Sequence->setDisabled(false);
		ui->actionExport_Textures->setDisabled(false);
		ui->actionLoadSkybox->setDisabled(true);
        menuBar()->insertAction(ui->menuLighting->menuAction(),ui->menuImage->menuAction());
	}else if (ViewerInterfaceType == VIEWER_INTERFACETYPE_SOUND){
		ui->actionLoadSkybox->setDisabled(true);
    }else if (ViewerInterfaceType == VIEWER_INTERFACETYPE_LANDSCAPE){
		ui->menuLighting->setDisabled(false);
		ui->menuCamera->setDisabled(false);
		ui->menuExport_Models->setDisabled(false);
		ui->actionSave_Screenshot->setDisabled(false);
		ui->actionSave_Image_Sequence->setDisabled(false);
		ui->actionExport_Textures->setDisabled(false);
        menuBar()->insertAction(ui->menuLighting->menuAction(),ui->menuLandscape->menuAction());
    }else if (ViewerInterfaceType == VIEWER_INTERFACETYPE_SET){
		ui->menuLighting->setDisabled(false);
		ui->menuCamera->setDisabled(false);
		ui->menuExport_Models->setDisabled(false);
		ui->actionSave_Screenshot->setDisabled(false);
		ui->actionSave_Image_Sequence->setDisabled(false);
		ui->actionExport_Textures->setDisabled(false);
        menuBar()->insertAction(ui->menuLighting->menuAction(),ui->menuSets->menuAction());
	}else if (ViewerInterfaceType == VIEWER_INTERFACETYPE_ITEM){
		ui->menuLighting->setDisabled(false);
		ui->menuCamera->setDisabled(false);
		ui->menuExport_Models->setDisabled(false);
		ui->actionSave_Screenshot->setDisabled(false);
		ui->actionSave_Image_Sequence->setDisabled(false);
		ui->actionExport_Textures->setDisabled(false);
    }else if (ViewerInterfaceType == VIEWER_INTERFACETYPE_CREATURE){
		ui->menuLighting->setDisabled(false);
		ui->menuCamera->setDisabled(false);
		ui->menuExport_Models->setDisabled(false);
		ui->actionSave_Screenshot->setDisabled(false);
		ui->actionSave_Image_Sequence->setDisabled(false);
		ui->actionExport_Textures->setDisabled(false);
        menuBar()->insertAction(ui->menuLighting->menuAction(),ui->menuNPC->menuAction());
    }else if (ViewerInterfaceType == VIEWER_INTERFACETYPE_CHARACTER){
		ui->menuLighting->setDisabled(false);
		ui->menuCamera->setDisabled(false);
		ui->menuExport_Models->setDisabled(false);
		ui->actionSave_Screenshot->setDisabled(false);
		ui->actionSave_Image_Sequence->setDisabled(false);
		ui->actionExport_Textures->setDisabled(false);
        menuBar()->insertAction(ui->menuLighting->menuAction(),ui->menuCharacter->menuAction());
    }else if (ViewerInterfaceType == VIEWER_INTERFACETYPE_NONE){
    }

	// Clear and Add back all the WoW Directories...
	ui->menuChange_WoW_Directory->addAction(ui->actionManage_Directories);
	ui->menuChange_WoW_Directory->addSeparator();
	/*
	if (WoWDirList.count() == 0){
		QLOG_INFO() << "No WoW Directories loaded. No actions listed.";
		QAction actionNone(iconDisabled,"None",this);
		actionNone.setDisabled(true);
		WoWDirGroup->addAction(&actionNone);
		ui->menuChange_WoW_Directory->addAction(&actionNone);
	}else{
		QLOG_INFO() << "Adding WoW Directory Listings...";

		QMap<int,st_WoWDir> sortlist;
		for (QMap<QString,st_WoWDir>::Iterator i=WoWDirList.begin();i!=WoWDirList.end();i++){
			st_WoWDir a = i.value();
			if (a == st_WoWDir())
				continue;
			sortlist.insertMulti(a.Position,a);	// Used Multi just in case. We don't want to overwrite directories with the same position number.
		}

		for (size_t i=0;i<sortlist.size();i++){
			st_WoWDir a = sortlist.value((int)i);
			QString g = WoWDirGroupName(a);
			QString loc = LocaleList.value(a.Locale);

			QAction *newAction = new QAction(a.Name,this);
			newAction->setCheckable(true);
			newAction->setActionGroup(WoWDirGroup);
			newAction->setText(QString("%1, %2").arg(a.Name,loc));
			newAction->setData(g);
			connect(newAction, SIGNAL(triggered()), this, SLOT(updateCurrentDirfromMenu()));

			// Catch-all/Default Icon. Should always be the latest release's icon.
			newAction->setIcon(iconCata);

			if (a.Version <= WOW_VANILLA){
				newAction->setIcon(iconVanilla);
			}else if (a.Version <= WOW_TBC){
				newAction->setIcon(iconTBC);
			}else if (a.Version <= WOW_WOTLK){
				newAction->setIcon(iconWotLK);
			}else if (a.Version <= WOW_CATACLYSM){
				newAction->setIcon(iconCata);
			}else if ((a.Version <= WOW_BETA) || (a.Version == WOW_PTR)){
				newAction->setIcon(iconPTR);
			}

			newAction->setChecked(false);
			if (CurrentDir == a)
				newAction->setChecked(true);
			
			ui->menuChange_WoW_Directory->addAction(newAction);
		}
	}
	*/

	//ChangeLoadButton();

	// Disable Temps if WoW is not Loaded
	if (isWoWLoaded == true){
		ui->rBtn_IsChar->setDisabled(false);
		ui->rBtn_IsADT->setDisabled(false);
		ui->rBtn_IsItem->setDisabled(false);
		ui->rBtn_IsNPC->setDisabled(false);
		ui->rBtn_IsSound->setDisabled(false);
		ui->rBtn_IsTexture->setDisabled(false);
		ui->rBtn_IsWMO->setDisabled(false);
		ui->rBtn_NoModel->setDisabled(false);
	}else{
		ui->rBtn_IsChar->setDisabled(true);
		ui->rBtn_IsADT->setDisabled(true);
		ui->rBtn_IsItem->setDisabled(true);
		ui->rBtn_IsNPC->setDisabled(true);
		ui->rBtn_IsSound->setDisabled(true);
		ui->rBtn_IsTexture->setDisabled(true);
		ui->rBtn_IsWMO->setDisabled(true);
		ui->rBtn_NoModel->setDisabled(true);
	}

	this->update();
	QLOG_INFO() << "Finished Updating the Viewer Menu.";
}

// Close WMV from File Menu
void Interface_Viewer::on_actionExit_triggered()
{
    QApplication::quit();
}

// Load World of Warcraft
void Interface_Viewer::LoadWoW()
{
	// Set Global Variables
	WoWTypeCurrent = WoWTypeNext;
	WoWTypeNext = WOW_NOTLOADED;
	isWoWLoaded = true;
	canReloadWoW = false;

	// Update the Menu
    UpdateViewerMenu();
}

// Unload World of Warcraft
void Interface_Viewer::UnLoadWoW()
{
	// Set Global Variables
	WoWTypeCurrent = WOW_NOTLOADED;
	WoWTypeNext = WOW_NOTLOADED;
	isWoWLoaded = false;
	canReloadWoW = false;

	// Update the Menu
    UpdateViewerMenu();
}

// Menu function for Loading World of Warcraft
void Interface_Viewer::on_actionLoad_World_of_Wacraft_triggered()
{
	LoadWoW();
}

// This, sadly will have to be OS-dependant...
void Interface_Viewer::on_actionView_Log_triggered()
{
	QProcess *openProgram = new QProcess();
	QString file(QDir().absoluteFilePath(LOGFILE_FILENAME));
	
	QMessageBox err;
	err.setIcon(QMessageBox::Critical);

	if (QFile(file).exists() == false){
		QLOG_ERROR() << "Amazingly, this log file was not found!";		// Yes, I know logging this is redunant and possibly stupid, but you never know...
		updateStatusBar(tr("Error: Log file not found. Please check for it's existance."));
		err.setWindowTitle(tr("Error: Log file not found!"));
		err.setText(tr("Could not find the %1 file.\n\nCheck your permissions, as you might be running WMV in a location that does not allow for the generation of files.").arg(LOGFILE_FILENAME));
		err.exec();
		return;
	}

#ifdef _WINDOWS
	// Windows will open the file in Notepad.
	openProgram->start("notepad.exe \""+file+"\"",QIODevice::ReadOnly);
	QLOG_INFO() << "Opening Notepad to load" << file;
#elif defined (_MAC)
	// Mac will open the file using TextEdit
	openProgram->start("/Applications/TextEdit.app/Contents/MacOS/TextEdit \""+file+"\"",QIODevice::ReadOnly);
	QLOG_INFO() << "Opening TextEdit to load" << file;
#elif defined (_LINUX)
	// Linux might require some more juggling, depending on several factors, some of which are defined in the constants.h file.
	prog = LINUX_TXT_VIEWER;
	openProgram->start(LINUX_TXT_VIEWER+" \""+file+"\"",QIODevice::ReadOnly);
	QLOG_INFO() << "Opening" << static_cast<const char*>(LINUX_TXT_VIEWER) << "to load" << file;
#endif

	if (!openProgram->waitForStarted())
	{
		QLOG_ERROR() << "An error occured while trying to open the log file.";
		updateStatusBar(tr("There was an error while trying to open your text viewer."));
		err.setWindowTitle(tr("Error opening Log file..."));
		err.setText(tr("Could not open the log file for an unknown reason.\n\nPlease navigate to %1 and open the file manually.").arg(file));
		err.exec();
		return;
	}
	QLOG_INFO() << "Successfully opened the log file.";
}

// Connect the two Initial Pose Only checkboxes.
void Interface_Viewer::on_actionInitial_Pose_Only_toggled(bool arg1)
{
    ui->actionInitial_Pose_Only_2->setChecked(arg1);
}
void Interface_Viewer::on_actionInitial_Pose_Only_2_toggled(bool arg1)
{
    ui->actionInitial_Pose_Only->setChecked(arg1);
}

// Show Controls
void Interface_Viewer::on_actionShowCtrl_FileList_toggled(bool arg1)
{
	if (arg1 == false){
		ui->ViewerFileListDockWidget->hide();
	}else{
		ui->ViewerFileListDockWidget->show();
	}
}

// Update Current Dir
void Interface_Viewer::updateCurrentDirfromMenu(){
	/*

	QLOG_INFO() << "Changing Current Directory from menu...";
	QString dirname = "None";
	QString olddir = sWMVSettings.value("CurrentWoWDir").toString();

	for (size_t i=0;i<WoWDirGroup->actions().count();i++)
	{
		QAction *a = WoWDirGroup->actions().value((int)i);

		if (a->isChecked()){
			dirname = a->data().toString();
			break;
		}
	}

	if (olddir == dirname){
		QLOG_INFO() << "Current Directory already set.";
		return;
	}

	st_WoWDir b(WoWDirList.value(dirname));

	QLOG_INFO() << "Setting Current Directory...";
	CurrentDir = WoWDirList.value(dirname);
	sWMVSettings.setValue("CurrentWoWDir",dirname);
	sWMVSettings.sync();

	WoWTypeNext = b.Version;
	canReloadWoW = true;

	*/
}

// Open WoW Directory Manager
void Interface_Viewer::on_actionManage_Directories_triggered()
{
	WoWDirectoryManager.init();
	WoWDirectoryManager.show();
}

// Open About Window
void Interface_Viewer::on_actionAbout_triggered()
{
	Window_About.show();
}

// Open About Plugins Window
void Interface_Viewer::on_actionAbout_Plugins_triggered()
{
	Window_AboutPlugins.init();
	Window_AboutPlugins.show();
}


// Functions for the temporary radio buttons on the main window.
void Interface_Viewer::on_rBtn_NoModel_clicked()
{
    ViewerInterfaceType = VIEWER_INTERFACETYPE_NONE;
    UpdateViewerMenu();
}

void Interface_Viewer::on_rBtn_IsChar_clicked()
{
    ViewerInterfaceType = VIEWER_INTERFACETYPE_CHARACTER;
    UpdateViewerMenu();
}

void Interface_Viewer::on_rBtn_IsNPC_clicked()
{
    ViewerInterfaceType = VIEWER_INTERFACETYPE_CREATURE;
    UpdateViewerMenu();
}

void Interface_Viewer::on_rBtn_IsWMO_clicked()
{
    ViewerInterfaceType = VIEWER_INTERFACETYPE_SET;
    UpdateViewerMenu();
}

void Interface_Viewer::on_rBtn_IsADT_clicked()
{
    ViewerInterfaceType = VIEWER_INTERFACETYPE_LANDSCAPE;
    UpdateViewerMenu();
}

void Interface_Viewer::on_rBtn_IsTexture_clicked()
{
    ViewerInterfaceType = VIEWER_INTERFACETYPE_IMAGE;
    UpdateViewerMenu();
}

void Interface_Viewer::on_rBtn_IsItem_clicked()
{
    ViewerInterfaceType = VIEWER_INTERFACETYPE_ITEM;
    UpdateViewerMenu();
}

void Interface_Viewer::on_rBtn_IsSound_clicked()
{
    ViewerInterfaceType = VIEWER_INTERFACETYPE_SOUND;
    UpdateViewerMenu();
}

#include <QProcess>
#include "application.h"
#include "Settings_Main.h"
#include "Exporters\Settings_Exporters.h"
#include "Exporters\exporters.h"

// These are temporarly added, just to test the files.
// Move these to the proper file when we get to implementing them.
#include "Model_M2.h"
#include "Model_WMO.h"
#include "Model_ADT.h"

// About Window
WindowAbout::WindowAbout(QWidget *parent) : QDialog(parent), ui_About(new Ui::AboutWindow)
{
	// Setup Window
	ui_About->setupUi(this);

	// Set Various Variables
	ui_About->WMVName->setText(PROGRAMNAME);
	ui_About->VersionNumber->setText(ui_About->VersionNumber->text().arg(MAJORVERSION,BUILDVERSION));
	ui_About->Edition->setText(ui_About->Edition->text().arg(SYSTEMVERSION,DEBUGVERSION));
	ui_About->ContentCopyright->setText(ui_About->ContentCopyright->text().arg(QDate().currentDate().toString("yyyy")));
}
WindowAbout::~WindowAbout()
{
	delete ui_About;
}

// Main WoW Model Viewer
WoWModelViewer::WoWModelViewer(QWidget *parent) : QMainWindow(parent), ui(new Ui::Main_Window_Viewer)
{
	// Setup the UI
    ui->setupUi(this);

	// Set Application Data
	QCoreApplication::setApplicationName(PROGRAMNAME);
	QCoreApplication::setApplicationVersion(MajorBuildVersion);
	QCoreApplication::setOrganizationName(ORGANIZATIONNAME);
	QCoreApplication::setOrganizationDomain(ORGANIZATIONWEBSITE);
	
	// Icons
	iconVanilla.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon1-Vanilla"), QSize(), QIcon::Normal, QIcon::Off);
    iconVanilla.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon1-Vanilla"), QSize(), QIcon::Normal, QIcon::On);
    iconTBC.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon2-TBC"), QSize(), QIcon::Normal, QIcon::Off);
    iconTBC.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon2-TBC"), QSize(), QIcon::Normal, QIcon::On);
    iconWotLK.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon3-WotLK"), QSize(), QIcon::Normal, QIcon::Off);
	iconWotLK.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon3-WotLK"), QSize(), QIcon::Normal, QIcon::On);
    iconCata.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon4-Cataclysm"), QSize(), QIcon::Normal, QIcon::Off);
	iconCata.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon4-Cataclysm"), QSize(), QIcon::Normal, QIcon::On);
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
	setWindowTitle(QString("%1 %2 %3 %4%5").arg(PROGRAMNAME,MAJORVERSION,BUILDVERSION,SYSTEMVERSION,DEBUGVERSION));

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


void WoWModelViewer::init()
{
	Exporters::Exporters();	// Initialize the Exporters
	CheckSettings_Main();	// Check Main Program Settings
	ReadWoWDirList();		// Read the WoW Directory List

	// Dependant Defaults
	InterfaceMode = sWMVSettings.value("LastInterfaceMode").toUInt();	// Set the default mode to Viewer.
	CurrentDir = WoWDirList.value(SettingsList.value("CurrentWoWDir").toString(),st_WoWDir());
	WoWTypeCurrent = CurrentDir.Version;								// The currently loaded World of Warcraft type.

	QLOG_INFO() << "Current WoW Directory:" << sWMVSettings.value("CurrentWoWDir").toString();
	QLOG_INFO() << "Current WoW Version:" << CurrentDir.Version;

	UpdateViewerMenu();

	// Build Status Bars
	createStatusBar();
}

void WoWModelViewer::createStatusBar()
{
	statusBar()->setAutoFillBackground(true);
	statusBar()->setSizeGripEnabled(false);
	statusBar()->showMessage(tr("Ready"));
}

void WoWModelViewer::updateStatusBar(QString newstatus)
{
	statusBar()->showMessage(newstatus);
}

WoWModelViewer::~WoWModelViewer()
{
    delete ui;
}

void WoWModelViewer::UpdateViewerMenu(){
    // Remove Menus
    menuBar()->removeAction(ui->menuCharacter->menuAction());
    menuBar()->removeAction(ui->menuNPC->menuAction());
    menuBar()->removeAction(ui->menuSets->menuAction());
    menuBar()->removeAction(ui->menuLandscape->menuAction());
    menuBar()->removeAction(ui->menuImage->menuAction());

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
		return;
	}

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

	ChangeLoadButton();

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

	QAction actionNone(iconDisabled,"None",this);
	actionNone.setDisabled(true);
	if (WoWDirList.count() == 0){
		QLOG_INFO() << "No WoW Directories loaded. No actions listed.";
		WoWDirGroup->addAction(&actionNone);
		ui->menuChange_WoW_Directory->insertAction(ui->actionManage_Directories,&actionNone);
	}else{
		QLOG_INFO() << "Adding WoW Directory Listings...";
	}

	this->update();
}

void WoWModelViewer::updateFileList()
{
	// Update the File Tree, based on the selected filetype
	if (ui->ViewerFileTypeSelector->currentIndex() == FILETYPE_MODEL){
	}else if(ui->ViewerFileTypeSelector->currentIndex() == FILETYPE_SET){
	}else if(ui->ViewerFileTypeSelector->currentIndex() == FILETYPE_LANDSCAPE){
	}else if(ui->ViewerFileTypeSelector->currentIndex() == FILETYPE_IMAGE){
	}else if(ui->ViewerFileTypeSelector->currentIndex() == FILETYPE_SOUND){
	}
}

// Connect the two Initial Pose Only checkboxes.
void WoWModelViewer::on_actionInitial_Pose_Only_2_toggled(bool arg1)
{
    ui->actionInitial_Pose_Only->setChecked(arg1);
}
void WoWModelViewer::on_actionInitial_Pose_Only_toggled(bool arg1)
{
    ui->actionInitial_Pose_Only_2->setChecked(arg1);
}

// Show Controls
void WoWModelViewer::on_actionShowCtrl_FileList_toggled(bool arg1)
{
	if (arg1 == false){
		ui->ViewerFileListDockWidget->hide();
	}else{
		ui->ViewerFileListDockWidget->show();
	}
}

// Close WMV from File Menu
void WoWModelViewer::on_actionExit_triggered()
{
    QApplication::quit();
}

// Update Load WoW Button
void WoWModelViewer::UpdateLoadButton()
{
	UpdateViewerMenu();
}

// Update the Load WoW Button
void WoWModelViewer::ChangeLoadButton()
{
	// WoW Icons
	QIcon LoadWoW;
	LoadWoW.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon-Disabled"), QSize(), QIcon::Disabled, QIcon::Off);
    LoadWoW.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon-Disabled"), QSize(), QIcon::Disabled, QIcon::On);
	if (ui->actionWoWDir_Vanilla->isChecked()){
		WoWTypeNext = WOW_VANILLA;
		if ((isWoWLoaded == true) && (WoWTypeCurrent != WoWTypeNext))
			canReloadWoW = true;
		LoadWoW.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon1-Vanilla"), QSize(), QIcon::Normal, QIcon::Off);
        LoadWoW.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon1-Vanilla"), QSize(), QIcon::Normal, QIcon::On);
	}else if(ui->actionWoWDir_BC->isChecked()){
		WoWTypeNext = WOW_TBC;
		if ((isWoWLoaded == true) && (WoWTypeCurrent != WoWTypeNext))
			canReloadWoW = true;
		LoadWoW.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon2-TBC"), QSize(), QIcon::Normal, QIcon::Off);
        LoadWoW.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon2-TBC"), QSize(), QIcon::Normal, QIcon::On);
	}else if(ui->actionWoWDir_WotLK->isChecked()){
		WoWTypeNext = WOW_WOTLK;
		if ((isWoWLoaded == true) && (WoWTypeCurrent != WoWTypeNext))
			canReloadWoW = true;
		LoadWoW.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon3-WotLK"), QSize(), QIcon::Normal, QIcon::Off);
        LoadWoW.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon3-WotLK"), QSize(), QIcon::Normal, QIcon::On);
	}else if(ui->actionWoWDir_Cataclysm->isChecked()){
		WoWTypeNext = WOW_CATACLYSM;
		if ((isWoWLoaded == true) && (WoWTypeCurrent != WoWTypeNext))
				canReloadWoW = true;
		LoadWoW.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon4-Cataclysm"), QSize(), QIcon::Normal, QIcon::Off);
        LoadWoW.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon4-Cataclysm"), QSize(), QIcon::Normal, QIcon::On);
	}else if(ui->actionWoWDir_PTR->isChecked()){
		WoWTypeNext = WOW_PTR;
		if ((isWoWLoaded == true) && (WoWTypeCurrent != WoWTypeNext))
			canReloadWoW = true;
		LoadWoW.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon-PTR"), QSize(), QIcon::Normal, QIcon::Off);
        LoadWoW.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon-PTR"), QSize(), QIcon::Normal, QIcon::On);
	}else{
		// Default to the standard, just in case.
		LoadWoW.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon1-Vanilla"), QSize(), QIcon::Normal, QIcon::Off);
        LoadWoW.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon1-Vanilla"), QSize(), QIcon::Normal, QIcon::On);
	}
	ui->actionLoad_World_of_Wacraft->setIcon(LoadWoW);

	if ((canReloadWoW == true)||(isWoWLoaded == false)){
		ui->actionLoad_World_of_Wacraft->setDisabled(false);
	}else{
		ui->actionLoad_World_of_Wacraft->setDisabled(true);
	}
	
	menuBar()->update();
}

// Load World of Warcraft
void WoWModelViewer::on_actionLoad_World_of_Wacraft_triggered()
{
	// Load the New WoW

	// Set Global Variables
	WoWTypeCurrent = WoWTypeNext;
	WoWTypeNext = WOW_NOTLOADED;
	isWoWLoaded = true;
	canReloadWoW = false;

	// Update the Menu
    UpdateViewerMenu();
}

// This, sadly will have to be OS-dependant...
void WoWModelViewer::on_actionView_Log_triggered()
{
	QProcess *openProgram = new QProcess();
	QString file(QDir().absoluteFilePath(LOGTXTFILENAME));
	
	QMessageBox err;
	err.setIcon(QMessageBox::Critical);

	if (QFile(file).exists() == false){
		QLOG_ERROR() << "Amazingly, this log file was not found!";		// Yes, I know logging this is redunant and possibly stupid, but you never know...
		updateStatusBar(tr("Error: Log file not found. Please check for it's existance."));
		err.setWindowTitle(tr("Error: Log file not found!"));
		err.setText(tr("Could not find the %1 file.\n\nCheck your permissions, as you might be running WMV in a location that does not allow for the generation of files.").arg(LOGTXTFILENAME));
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

// Open WoW Directory Manager
void WoWModelViewer::on_actionManage_Directories_triggered()
{
	WoWDirectoryManager.init();
	WoWDirectoryManager.show();
}

// Open About Window
void WoWModelViewer::on_actionAbout_triggered()
{
	Window_About.show();
}



// Functions for the temporary radio buttons on the main window.
void WoWModelViewer::on_rBtn_NoModel_clicked()
{
    ViewerInterfaceType = VIEWER_INTERFACETYPE_NONE;
    UpdateViewerMenu();
}

void WoWModelViewer::on_rBtn_IsChar_clicked()
{
    ViewerInterfaceType = VIEWER_INTERFACETYPE_CHARACTER;
    UpdateViewerMenu();
}

void WoWModelViewer::on_rBtn_IsNPC_clicked()
{
    ViewerInterfaceType = VIEWER_INTERFACETYPE_CREATURE;
    UpdateViewerMenu();
}

void WoWModelViewer::on_rBtn_IsWMO_clicked()
{
    ViewerInterfaceType = VIEWER_INTERFACETYPE_SET;
    UpdateViewerMenu();
}

void WoWModelViewer::on_rBtn_IsADT_clicked()
{
    ViewerInterfaceType = VIEWER_INTERFACETYPE_LANDSCAPE;
    UpdateViewerMenu();
}

void WoWModelViewer::on_rBtn_IsTexture_clicked()
{
    ViewerInterfaceType = VIEWER_INTERFACETYPE_IMAGE;
    UpdateViewerMenu();
}

void WoWModelViewer::on_rBtn_IsItem_clicked()
{
    ViewerInterfaceType = VIEWER_INTERFACETYPE_ITEM;
    UpdateViewerMenu();
}

void WoWModelViewer::on_rBtn_IsSound_clicked()
{
    ViewerInterfaceType = VIEWER_INTERFACETYPE_SOUND;
    UpdateViewerMenu();
}

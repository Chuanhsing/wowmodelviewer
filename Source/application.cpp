#include "application.h"
#include "ui_Main_Window_Viewer.h"
#include "Settings_Main.h"
#include "Exporters\exporters.h"

// These are temporarly added, just to test the files.
// Move these to the proper file when we get to implementing them.
#include "Model_M2.h"
#include "Model_WMO.h"
#include "Model_ADT.h"

// About Window
WindowAbout::WindowAbout(QWidget *parent) : QDialog(parent), ui_About(new Ui::AboutWindow)
{
	ui_About->setupUi(this);

	ui_About->VersionNumber->setText(QString(MAJORVERSION + BUILDVERSION));
	ui_About->Edition->setText(QString(SYSTEMVERSION + DEBUGVERSION + tr(" Edition")));
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
	QCoreApplication::setApplicationVersion(MAJORVERSION + BUILDVERSION);
	QCoreApplication::setOrganizationName(ORGANIZATIONNAME);
	QCoreApplication::setOrganizationDomain(ORGANIZATIONWEBSITE);

	Exporters();			// Initialize the Exporters

	CheckSettings_Main();	// Check Main Program Settings
	
    // Defaults
	InterfaceMode = INTERFACEMODE_VIEWER;				// Set the default mode to Viewer.
    ViewerInterfaceType = VIEWER_INTERFACETYPE_NONE;	// Full list of viewer interface types in enums.h
	WoWTypeCurrent = WOW_VANILLA;						// The currently loaded World of Warcraft type.
	WoWTypeNext = WOW_NOTLOADED;						// The next World of Warcraft type that will be loaded.
	isWoWLoaded = false;
	canReloadWoW = false;

	// Set the main Window's Title
	setWindowTitle(PROGRAMNAME + tr(" ","WindowTitleSpace1") + MAJORVERSION + tr(" ","WindowTitleSpace2") + BUILDVERSION + tr(" ","WindowTitleSpace3") + SYSTEMVERSION + DEBUGVERSION);

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
	// For the Temp/Testing dirs
	WoWDirGroup = new QActionGroup(this);
	WoWDirGroup->addAction(ui->actionWoWDir_Vanilla);
	WoWDirGroup->addAction(ui->actionWoWDir_BC);
	WoWDirGroup->addAction(ui->actionWoWDir_WotLK);
	WoWDirGroup->addAction(ui->actionWoWDir_Cataclysm);
	WoWDirGroup->addAction(ui->actionWoWDir_PTR);
    ui->actionWoWDir_Vanilla->setChecked(true);

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
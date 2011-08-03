#include "application.h"
#include "ui_Main_Window_Viewer.h"

// These are temporarly added, just to test the files.
// Move these to the proper file when we get to implementing them.
#include "Model_M2.h"
#include "Model_WMO.h"
#include "Model_ADT.h"

WoWModelViewer::WoWModelViewer(QWidget *parent) : QMainWindow(parent), ui(new Ui::Main_Window_Viewer)
{
    ui->setupUi(this);

    // Defaults
	InterfaceMode = INTERFACEMODE_VIEWER;				// Set the default mode to Viewer.
    ViewerInterfaceType = VIEWER_INTERFACETYPE_NONE;	// Full list of viewer interface types in enums.h
	isWoWLoaded = false;
	canReloadWoW = false;

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

	// Build Status Bars
	createStatusBar();

    UpdateViewerMenu();
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


// Functions for the temporary radio buttons on the main window.
void WoWModelViewer::on_rBtn_WoWNotLoaded_clicked()
{
	isWoWLoaded = false;
    UpdateViewerMenu();
}

void WoWModelViewer::on_rBtn_NoModel_clicked()
{
	isWoWLoaded = true;
    ViewerInterfaceType = VIEWER_INTERFACETYPE_NONE;
    UpdateViewerMenu();
}

void WoWModelViewer::on_rBtn_IsChar_clicked()
{
	isWoWLoaded = true;
    ViewerInterfaceType = VIEWER_INTERFACETYPE_CHARACTER;
    UpdateViewerMenu();
}

void WoWModelViewer::on_rBtn_IsNPC_clicked()
{
	isWoWLoaded = true;
    ViewerInterfaceType = VIEWER_INTERFACETYPE_CREATURE;
    UpdateViewerMenu();
}

void WoWModelViewer::on_rBtn_IsWMO_clicked()
{
	isWoWLoaded = true;
    ViewerInterfaceType = VIEWER_INTERFACETYPE_SET;
    UpdateViewerMenu();
}

void WoWModelViewer::on_rBtn_IsADT_clicked()
{
	isWoWLoaded = true;
    ViewerInterfaceType = VIEWER_INTERFACETYPE_LANDSCAPE;
    UpdateViewerMenu();
}

void WoWModelViewer::on_rBtn_IsTexture_clicked()
{
	isWoWLoaded = true;
    ViewerInterfaceType = VIEWER_INTERFACETYPE_IMAGE;
    UpdateViewerMenu();
}

void WoWModelViewer::on_rBtn_IsItem_clicked()
{
	isWoWLoaded = true;
    ViewerInterfaceType = VIEWER_INTERFACETYPE_ITEM;
    UpdateViewerMenu();
}

void WoWModelViewer::on_rBtn_IsSound_clicked()
{
	isWoWLoaded = true;
    ViewerInterfaceType = VIEWER_INTERFACETYPE_SOUND;
    UpdateViewerMenu();
}

void WoWModelViewer::on_actionExit_triggered()
{
    QApplication::quit();
}
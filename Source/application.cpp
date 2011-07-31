#include "application.h"
#include "ui_Main_Window_Viewer.h"

WoWModelViewer::WoWModelViewer(QWidget *parent) : QMainWindow(parent), ui(new Ui::Main_Window_Viewer)
{
    ui->setupUi(this);

    // Defaults
	InterfaceMode = INTERFACEMODE_VIEWER;				// Set the default mode to Viewer.
    ViewerInterfaceType = VIEWER_INTERFACETYPE_NONE;	// Full list of viewer interface types in enums.h

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

	// Disable Model-Only Options
    ui->menuLighting->setDisabled(false);
    ui->menuCamera->setDisabled(false);
    ui->menuExport_Models->setDisabled(false);

    // Change Options and Insert Menus based on the File Type
    if (ViewerInterfaceType == VIEWER_INTERFACETYPE_IMAGE){
        menuBar()->insertAction(ui->menuLighting->menuAction(),ui->menuImage->menuAction());
        ui->menuLighting->setDisabled(true);
        ui->menuCamera->setDisabled(true);
        ui->menuExport_Models->setDisabled(true);
	}else if (ViewerInterfaceType == VIEWER_INTERFACETYPE_SOUND){
        ui->menuLighting->setDisabled(true);
        ui->menuCamera->setDisabled(true);
        ui->menuExport_Models->setDisabled(true);
    }else if (ViewerInterfaceType == VIEWER_INTERFACETYPE_LANDSCAPE){
        menuBar()->insertAction(ui->menuLighting->menuAction(),ui->menuLandscape->menuAction());
    }else if (ViewerInterfaceType == VIEWER_INTERFACETYPE_SET){
        menuBar()->insertAction(ui->menuLighting->menuAction(),ui->menuSets->menuAction());
	}else if (ViewerInterfaceType == VIEWER_INTERFACETYPE_ITEM){
    }else if (ViewerInterfaceType == VIEWER_INTERFACETYPE_CREATURE){
        menuBar()->insertAction(ui->menuLighting->menuAction(),ui->menuNPC->menuAction());
    }else if (ViewerInterfaceType == VIEWER_INTERFACETYPE_CHARACTER){
        menuBar()->insertAction(ui->menuLighting->menuAction(),ui->menuCharacter->menuAction());
    }else if (ViewerInterfaceType == VIEWER_INTERFACETYPE_NONE){
        ui->menuLighting->setDisabled(true);
        ui->menuCamera->setDisabled(true);
        ui->menuExport_Models->setDisabled(true);
    }

    // Update other Menus...
}

void WoWModelViewer::updateFileList()
{
	// Update the File Tree, based on the selected filetype
	if (ui->FileTypeSelector->currentIndex() == FILETYPE_MODEL){
	}else if(ui->FileTypeSelector->currentIndex() == FILETYPE_SET){
	}else if(ui->FileTypeSelector->currentIndex() == FILETYPE_LANDSCAPE){
	}else if(ui->FileTypeSelector->currentIndex() == FILETYPE_IMAGE){
	}else if(ui->FileTypeSelector->currentIndex() == FILETYPE_SOUND){
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

void WoWModelViewer::on_actionExit_triggered()
{
    QApplication::quit();
}
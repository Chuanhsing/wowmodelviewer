#include "mainwindow.h"
#include "ui_mainwindow.h"

WoWModelViewer::WoWModelViewer(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Defaults
    FileType = 0;   // 0 = No File Loaded. 1 = Char, 2 = NPC, 3 = WMO/Set, 4 = ADT/Landscape, 5 = Image

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
    CameraGroup->addAction(ui->actionCam_Model);
    CameraGroup->addAction(ui->actionCam_Additional);
    ui->actionCam_Perspective->setChecked(true);


    // Doodad Sets
    DoodadSetGroup = new QActionGroup(this);

    UpdateMenu();
}

WoWModelViewer::~WoWModelViewer()
{
    delete ui;
}

void WoWModelViewer::UpdateMenu(){
    // Remove Menus
    menuBar()->removeAction(ui->menuCharacter->menuAction());
    menuBar()->removeAction(ui->menuNPC->menuAction());
    menuBar()->removeAction(ui->menuSets->menuAction());
    menuBar()->removeAction(ui->menuLandscape->menuAction());
    menuBar()->removeAction(ui->menuImage->menuAction());
    ui->menuLighting->setDisabled(false);
    ui->menuCamera->setDisabled(false);
    ui->menuExport_Models->setDisabled(false);

    // Insert Menus based on Model Type
    if (FileType == 5){
        menuBar()->insertAction(ui->menuLighting->menuAction(),ui->menuImage->menuAction());
        ui->menuLighting->setDisabled(true);
        ui->menuCamera->setDisabled(true);
        ui->menuExport_Models->setDisabled(true);
    }else if (FileType == 4){
        menuBar()->insertAction(ui->menuLighting->menuAction(),ui->menuLandscape->menuAction());
    }else if (FileType == 3){
        menuBar()->insertAction(ui->menuLighting->menuAction(),ui->menuSets->menuAction());
    }else if (FileType == 2){
        menuBar()->insertAction(ui->menuLighting->menuAction(),ui->menuNPC->menuAction());
    }else if (FileType == 1){
        menuBar()->insertAction(ui->menuLighting->menuAction(),ui->menuCharacter->menuAction());
    }else if (FileType == 0){
        ui->menuLighting->setDisabled(true);
        ui->menuCamera->setDisabled(true);
        ui->menuExport_Models->setDisabled(true);
    }

    // Update other Menus...
}

void WoWModelViewer::on_actionInitial_Pose_Only_2_toggled(bool arg1)
{
    ui->actionInitial_Pose_Only->setChecked(arg1);
}

void WoWModelViewer::on_actionInitial_Pose_Only_toggled(bool arg1)
{
    ui->actionInitial_Pose_Only_2->setChecked(arg1);
}

void WoWModelViewer::on_rBtn_NoModel_clicked()
{
    FileType = 0;
    UpdateMenu();
}

void WoWModelViewer::on_rBtn_IsChar_clicked()
{
    FileType = 1;
    UpdateMenu();
}

void WoWModelViewer::on_rBtn_IsNPC_clicked()
{
    FileType = 2;
    UpdateMenu();
}

void WoWModelViewer::on_rBtn_IsWMO_clicked()
{
    FileType = 3;
    UpdateMenu();
}

void WoWModelViewer::on_rBtn_IsADT_clicked()
{
    FileType = 4;
    UpdateMenu();
}

void WoWModelViewer::on_rBtn_IsTexture_clicked()
{
    FileType = 5;
    UpdateMenu();
}

void WoWModelViewer::on_actionExit_triggered()
{
    QApplication::quit();
}

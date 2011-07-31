/********************************************************************************
** Form generated from reading UI file 'Main_Window_Viewer.ui'
**
** Created: Sat Jul 30 18:54:37 2011
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAIN_WINDOW_VIEWER_H
#define UI_MAIN_WINDOW_VIEWER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDockWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QStatusBar>
#include <QtGui/QTreeWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Main_Window_Viewer
{
public:
    QAction *actionLoad_World_of_Wacraft;
    QAction *actionView_Log;
    QAction *actionSave_File;
    QAction *actionSave_Screenshot;
    QAction *actionSave_Image_Sequence;
    QAction *actionExport_Textures;
    QAction *actionInitial_Pose_Only;
    QAction *actionExporterSettings;
    QAction *actionExport_3DSMAX;
    QAction *actionExport_Collada;
    QAction *actionExport_FBX;
    QAction *actionExport_Lightwave;
    QAction *actionExport_Milkshape;
    QAction *actionExport_OBJ;
    QAction *actionExport_M3;
    QAction *actionExport_Ogre_XML;
    QAction *actionExport_X3D;
    QAction *actionExport_X3D_in_XHTML;
    QAction *actionDiscover_Items;
    QAction *actionDiscover_NPCs;
    QAction *actionExit;
    QAction *actionHelp;
    QAction *actionAbout;
    QAction *actionAlways_show_Default_Doodads;
    QAction *actionSettings;
    QAction *actionExporter_Settings_2;
    QAction *actionInitial_Pose_Only_2;
    QAction *actionUpdate_Databases;
    QAction *actionUpdate_Program;
    QAction *actionShow_WMOs;
    QAction *actionShow_Doodads;
    QAction *actionADTQualityHigh;
    QAction *actionADTQualityLow;
    QAction *actionLoad_Character;
    QAction *actionSave_Character;
    QAction *actionRandomize_Character;
    QAction *actionLoad_Equipment;
    QAction *actionSave_Equipment;
    QAction *actionClear_Equiptment;
    QAction *actionLoad_Equipment_Set;
    QAction *actionLoad_Starting_Outfit;
    QAction *actionLoad_NPC_Outfit;
    QAction *actionGlowNone;
    QAction *actionGlowDefault;
    QAction *actionGlowDeathKnight;
    QAction *actionShow_Underwear;
    QAction *actionShow_Ears;
    QAction *actionShow_Hair;
    QAction *actionShow_Facial_Hair;
    QAction *actionShow_Feet;
    QAction *actionSheath_Weapons;
    QAction *actionLoad_a_Mount;
    QAction *actionLoad_NPC;
    QAction *actionAdjust_Lighting;
    QAction *actionShowCtrl_Animation;
    QAction *actionShowCtrl_Character;
    QAction *actionShowCtrl_Lighting;
    QAction *actionShowCtrl_Model;
    QAction *actionModel_Bank;
    QAction *actionSave_to_Model_Bank;
    QAction *actionShowCtrl_FileList;
    QAction *actionBackground_Color;
    QAction *actionBackground_Image;
    QAction *actionSkybox;
    QAction *actionShow_Grid;
    QAction *actionShow_Mask;
    QAction *actionCam_Perspective;
    QAction *actionCam_Top;
    QAction *actionCam_Left;
    QAction *actionCam_Right;
    QAction *actionCam_Back;
    QAction *actionCam_Model;
    QAction *actionSave_Masks;
    QAction *actionShow_Doodads_2;
    QAction *actionLoad_Lighting;
    QAction *actionSave_Lighting;
    QAction *actionRender_Light_Objects;
    QAction *actionUse_True_Lighting;
    QAction *actionLightType_Dynamic;
    QAction *actionLightType_Ambient;
    QAction *actionLightType_ModelOnly;
    QAction *actionCanvasSize11_0120x0120;
    QAction *actionCanvasSize11_0256x0256;
    QAction *actionCanvasSize11_0512x0512;
    QAction *actionCanvasSize11_1024x1024;
    QAction *actionCanvasSize43_0640x0480;
    QAction *actionCanvasSize43_0800x0600;
    QAction *actionCanvasSize43_1024x0768;
    QAction *actionCanvasSize43_1280x1024;
    QAction *actionCanvasSize43_1600x1200;
    QAction *actionCanvasSize169_0864x0480;
    QAction *actionCanvasSize169_1280x0720;
    QAction *actionCanvasSize169_1920x1080;
    QAction *actionCanvasSizeMisc_1280x0768;
    QAction *actionCanvasSizeMisc_1920x1200;
    QAction *actionApply_Enchants;
    QAction *actionLeft_Hand_Item;
    QAction *actionRight_Hand_Item;
    QAction *actionApply_Enchants_2;
    QAction *action_List_of_Doodad_Sets;
    QAction *actionCam_Additional;
    QAction *actionImage_ShowAlpha;
    QAction *actionDisplayImageInfo;
    QWidget *centralWidget;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QRadioButton *rBtn_IsChar;
    QRadioButton *rBtn_IsNPC;
    QRadioButton *rBtn_IsWMO;
    QRadioButton *rBtn_IsADT;
    QRadioButton *rBtn_IsTexture;
    QRadioButton *rBtn_NoModel;
    QRadioButton *rBtn_IsSound;
    QRadioButton *rBtn_IsItem;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuExport_Models;
    QMenu *menuCharacter;
    QMenu *menuEye_Glow;
    QMenu *menuLighting;
    QMenu *menuUse_Lights;
    QMenu *menuOptions;
    QMenu *menuAbout;
    QMenu *menuSets;
    QMenu *menuDoodad_Set;
    QMenu *menuLandscape;
    QMenu *menuADT_Quality;
    QMenu *menuView;
    QMenu *menuShow_Controls;
    QMenu *menuCamera;
    QMenu *menuSet_Canvas_Size;
    QMenu *menuNPC;
    QMenu *menuImage;
    QStatusBar *statusBar;
    QDockWidget *FileListDockWidget;
    QWidget *FileListDockWidgetContents;
    QTreeWidget *WoWFileTree;
    QComboBox *FileTypeSelector;
    QLineEdit *FilterEditBox;
    QLabel *Filterlabel;
    QPushButton *ClearFilterButton;

    void setupUi(QMainWindow *Main_Window_Viewer)
    {
        if (Main_Window_Viewer->objectName().isEmpty())
            Main_Window_Viewer->setObjectName(QString::fromUtf8("Main_Window_Viewer"));
        Main_Window_Viewer->resize(1024, 768);
        Main_Window_Viewer->setWindowTitle(QString::fromUtf8("WoW Model Viewer"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Resources/FileIcons/wmv.ico"), QSize(), QIcon::Normal, QIcon::Off);
        Main_Window_Viewer->setWindowIcon(icon);
        actionLoad_World_of_Wacraft = new QAction(Main_Window_Viewer);
        actionLoad_World_of_Wacraft->setObjectName(QString::fromUtf8("actionLoad_World_of_Wacraft"));
        actionView_Log = new QAction(Main_Window_Viewer);
        actionView_Log->setObjectName(QString::fromUtf8("actionView_Log"));
        actionSave_File = new QAction(Main_Window_Viewer);
        actionSave_File->setObjectName(QString::fromUtf8("actionSave_File"));
        actionSave_Screenshot = new QAction(Main_Window_Viewer);
        actionSave_Screenshot->setObjectName(QString::fromUtf8("actionSave_Screenshot"));
        actionSave_Image_Sequence = new QAction(Main_Window_Viewer);
        actionSave_Image_Sequence->setObjectName(QString::fromUtf8("actionSave_Image_Sequence"));
        actionExport_Textures = new QAction(Main_Window_Viewer);
        actionExport_Textures->setObjectName(QString::fromUtf8("actionExport_Textures"));
        actionInitial_Pose_Only = new QAction(Main_Window_Viewer);
        actionInitial_Pose_Only->setObjectName(QString::fromUtf8("actionInitial_Pose_Only"));
        actionInitial_Pose_Only->setCheckable(true);
        actionInitial_Pose_Only->setChecked(true);
        actionExporterSettings = new QAction(Main_Window_Viewer);
        actionExporterSettings->setObjectName(QString::fromUtf8("actionExporterSettings"));
        actionExport_3DSMAX = new QAction(Main_Window_Viewer);
        actionExport_3DSMAX->setObjectName(QString::fromUtf8("actionExport_3DSMAX"));
        actionExport_Collada = new QAction(Main_Window_Viewer);
        actionExport_Collada->setObjectName(QString::fromUtf8("actionExport_Collada"));
        actionExport_Collada->setEnabled(false);
        actionExport_FBX = new QAction(Main_Window_Viewer);
        actionExport_FBX->setObjectName(QString::fromUtf8("actionExport_FBX"));
        actionExport_Lightwave = new QAction(Main_Window_Viewer);
        actionExport_Lightwave->setObjectName(QString::fromUtf8("actionExport_Lightwave"));
        actionExport_Milkshape = new QAction(Main_Window_Viewer);
        actionExport_Milkshape->setObjectName(QString::fromUtf8("actionExport_Milkshape"));
        actionExport_OBJ = new QAction(Main_Window_Viewer);
        actionExport_OBJ->setObjectName(QString::fromUtf8("actionExport_OBJ"));
        actionExport_M3 = new QAction(Main_Window_Viewer);
        actionExport_M3->setObjectName(QString::fromUtf8("actionExport_M3"));
        actionExport_Ogre_XML = new QAction(Main_Window_Viewer);
        actionExport_Ogre_XML->setObjectName(QString::fromUtf8("actionExport_Ogre_XML"));
        actionExport_X3D = new QAction(Main_Window_Viewer);
        actionExport_X3D->setObjectName(QString::fromUtf8("actionExport_X3D"));
        actionExport_X3D_in_XHTML = new QAction(Main_Window_Viewer);
        actionExport_X3D_in_XHTML->setObjectName(QString::fromUtf8("actionExport_X3D_in_XHTML"));
        actionDiscover_Items = new QAction(Main_Window_Viewer);
        actionDiscover_Items->setObjectName(QString::fromUtf8("actionDiscover_Items"));
        actionDiscover_NPCs = new QAction(Main_Window_Viewer);
        actionDiscover_NPCs->setObjectName(QString::fromUtf8("actionDiscover_NPCs"));
        actionExit = new QAction(Main_Window_Viewer);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionExit->setMenuRole(QAction::QuitRole);
        actionHelp = new QAction(Main_Window_Viewer);
        actionHelp->setObjectName(QString::fromUtf8("actionHelp"));
        actionAbout = new QAction(Main_Window_Viewer);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionAbout->setMenuRole(QAction::AboutRole);
        actionAlways_show_Default_Doodads = new QAction(Main_Window_Viewer);
        actionAlways_show_Default_Doodads->setObjectName(QString::fromUtf8("actionAlways_show_Default_Doodads"));
        actionAlways_show_Default_Doodads->setCheckable(true);
        actionAlways_show_Default_Doodads->setChecked(true);
        actionSettings = new QAction(Main_Window_Viewer);
        actionSettings->setObjectName(QString::fromUtf8("actionSettings"));
        actionSettings->setMenuRole(QAction::PreferencesRole);
        actionExporter_Settings_2 = new QAction(Main_Window_Viewer);
        actionExporter_Settings_2->setObjectName(QString::fromUtf8("actionExporter_Settings_2"));
        actionInitial_Pose_Only_2 = new QAction(Main_Window_Viewer);
        actionInitial_Pose_Only_2->setObjectName(QString::fromUtf8("actionInitial_Pose_Only_2"));
        actionInitial_Pose_Only_2->setCheckable(true);
        actionInitial_Pose_Only_2->setChecked(true);
        actionUpdate_Databases = new QAction(Main_Window_Viewer);
        actionUpdate_Databases->setObjectName(QString::fromUtf8("actionUpdate_Databases"));
        actionUpdate_Program = new QAction(Main_Window_Viewer);
        actionUpdate_Program->setObjectName(QString::fromUtf8("actionUpdate_Program"));
        actionShow_WMOs = new QAction(Main_Window_Viewer);
        actionShow_WMOs->setObjectName(QString::fromUtf8("actionShow_WMOs"));
        actionShow_WMOs->setCheckable(true);
        actionShow_WMOs->setChecked(true);
        actionShow_Doodads = new QAction(Main_Window_Viewer);
        actionShow_Doodads->setObjectName(QString::fromUtf8("actionShow_Doodads"));
        actionShow_Doodads->setCheckable(true);
        actionShow_Doodads->setChecked(true);
        actionADTQualityHigh = new QAction(Main_Window_Viewer);
        actionADTQualityHigh->setObjectName(QString::fromUtf8("actionADTQualityHigh"));
        actionADTQualityHigh->setCheckable(true);
        actionADTQualityHigh->setChecked(true);
        actionADTQualityHigh->setMenuRole(QAction::PreferencesRole);
        actionADTQualityLow = new QAction(Main_Window_Viewer);
        actionADTQualityLow->setObjectName(QString::fromUtf8("actionADTQualityLow"));
        actionADTQualityLow->setCheckable(true);
        actionLoad_Character = new QAction(Main_Window_Viewer);
        actionLoad_Character->setObjectName(QString::fromUtf8("actionLoad_Character"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Resources/Images/Character_Load.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionLoad_Character->setIcon(icon1);
        actionSave_Character = new QAction(Main_Window_Viewer);
        actionSave_Character->setObjectName(QString::fromUtf8("actionSave_Character"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Resources/Images/Character_Save.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave_Character->setIcon(icon2);
        actionRandomize_Character = new QAction(Main_Window_Viewer);
        actionRandomize_Character->setObjectName(QString::fromUtf8("actionRandomize_Character"));
        actionLoad_Equipment = new QAction(Main_Window_Viewer);
        actionLoad_Equipment->setObjectName(QString::fromUtf8("actionLoad_Equipment"));
        actionSave_Equipment = new QAction(Main_Window_Viewer);
        actionSave_Equipment->setObjectName(QString::fromUtf8("actionSave_Equipment"));
        actionClear_Equiptment = new QAction(Main_Window_Viewer);
        actionClear_Equiptment->setObjectName(QString::fromUtf8("actionClear_Equiptment"));
        actionLoad_Equipment_Set = new QAction(Main_Window_Viewer);
        actionLoad_Equipment_Set->setObjectName(QString::fromUtf8("actionLoad_Equipment_Set"));
        actionLoad_Starting_Outfit = new QAction(Main_Window_Viewer);
        actionLoad_Starting_Outfit->setObjectName(QString::fromUtf8("actionLoad_Starting_Outfit"));
        actionLoad_NPC_Outfit = new QAction(Main_Window_Viewer);
        actionLoad_NPC_Outfit->setObjectName(QString::fromUtf8("actionLoad_NPC_Outfit"));
        actionGlowNone = new QAction(Main_Window_Viewer);
        actionGlowNone->setObjectName(QString::fromUtf8("actionGlowNone"));
        actionGlowNone->setCheckable(true);
        actionGlowDefault = new QAction(Main_Window_Viewer);
        actionGlowDefault->setObjectName(QString::fromUtf8("actionGlowDefault"));
        actionGlowDefault->setCheckable(true);
        actionGlowDefault->setChecked(true);
        actionGlowDeathKnight = new QAction(Main_Window_Viewer);
        actionGlowDeathKnight->setObjectName(QString::fromUtf8("actionGlowDeathKnight"));
        actionGlowDeathKnight->setCheckable(true);
        actionShow_Underwear = new QAction(Main_Window_Viewer);
        actionShow_Underwear->setObjectName(QString::fromUtf8("actionShow_Underwear"));
        actionShow_Underwear->setCheckable(true);
        actionShow_Underwear->setChecked(true);
        actionShow_Ears = new QAction(Main_Window_Viewer);
        actionShow_Ears->setObjectName(QString::fromUtf8("actionShow_Ears"));
        actionShow_Ears->setCheckable(true);
        actionShow_Ears->setChecked(true);
        actionShow_Hair = new QAction(Main_Window_Viewer);
        actionShow_Hair->setObjectName(QString::fromUtf8("actionShow_Hair"));
        actionShow_Hair->setCheckable(true);
        actionShow_Hair->setChecked(true);
        actionShow_Facial_Hair = new QAction(Main_Window_Viewer);
        actionShow_Facial_Hair->setObjectName(QString::fromUtf8("actionShow_Facial_Hair"));
        actionShow_Facial_Hair->setCheckable(true);
        actionShow_Facial_Hair->setChecked(true);
        actionShow_Feet = new QAction(Main_Window_Viewer);
        actionShow_Feet->setObjectName(QString::fromUtf8("actionShow_Feet"));
        actionShow_Feet->setCheckable(true);
        actionShow_Feet->setChecked(true);
        actionSheath_Weapons = new QAction(Main_Window_Viewer);
        actionSheath_Weapons->setObjectName(QString::fromUtf8("actionSheath_Weapons"));
        actionLoad_a_Mount = new QAction(Main_Window_Viewer);
        actionLoad_a_Mount->setObjectName(QString::fromUtf8("actionLoad_a_Mount"));
        actionLoad_NPC = new QAction(Main_Window_Viewer);
        actionLoad_NPC->setObjectName(QString::fromUtf8("actionLoad_NPC"));
        actionAdjust_Lighting = new QAction(Main_Window_Viewer);
        actionAdjust_Lighting->setObjectName(QString::fromUtf8("actionAdjust_Lighting"));
        actionShowCtrl_Animation = new QAction(Main_Window_Viewer);
        actionShowCtrl_Animation->setObjectName(QString::fromUtf8("actionShowCtrl_Animation"));
        actionShowCtrl_Animation->setCheckable(true);
        actionShowCtrl_Character = new QAction(Main_Window_Viewer);
        actionShowCtrl_Character->setObjectName(QString::fromUtf8("actionShowCtrl_Character"));
        actionShowCtrl_Character->setCheckable(true);
        actionShowCtrl_Lighting = new QAction(Main_Window_Viewer);
        actionShowCtrl_Lighting->setObjectName(QString::fromUtf8("actionShowCtrl_Lighting"));
        actionShowCtrl_Lighting->setCheckable(true);
        actionShowCtrl_Model = new QAction(Main_Window_Viewer);
        actionShowCtrl_Model->setObjectName(QString::fromUtf8("actionShowCtrl_Model"));
        actionShowCtrl_Model->setCheckable(true);
        actionModel_Bank = new QAction(Main_Window_Viewer);
        actionModel_Bank->setObjectName(QString::fromUtf8("actionModel_Bank"));
        actionSave_to_Model_Bank = new QAction(Main_Window_Viewer);
        actionSave_to_Model_Bank->setObjectName(QString::fromUtf8("actionSave_to_Model_Bank"));
        actionShowCtrl_FileList = new QAction(Main_Window_Viewer);
        actionShowCtrl_FileList->setObjectName(QString::fromUtf8("actionShowCtrl_FileList"));
        actionShowCtrl_FileList->setCheckable(true);
        actionShowCtrl_FileList->setChecked(true);
        actionBackground_Color = new QAction(Main_Window_Viewer);
        actionBackground_Color->setObjectName(QString::fromUtf8("actionBackground_Color"));
        actionBackground_Image = new QAction(Main_Window_Viewer);
        actionBackground_Image->setObjectName(QString::fromUtf8("actionBackground_Image"));
        actionSkybox = new QAction(Main_Window_Viewer);
        actionSkybox->setObjectName(QString::fromUtf8("actionSkybox"));
        actionShow_Grid = new QAction(Main_Window_Viewer);
        actionShow_Grid->setObjectName(QString::fromUtf8("actionShow_Grid"));
        actionShow_Grid->setCheckable(true);
        actionShow_Mask = new QAction(Main_Window_Viewer);
        actionShow_Mask->setObjectName(QString::fromUtf8("actionShow_Mask"));
        actionShow_Mask->setCheckable(true);
        actionCam_Perspective = new QAction(Main_Window_Viewer);
        actionCam_Perspective->setObjectName(QString::fromUtf8("actionCam_Perspective"));
        actionCam_Perspective->setCheckable(true);
        actionCam_Top = new QAction(Main_Window_Viewer);
        actionCam_Top->setObjectName(QString::fromUtf8("actionCam_Top"));
        actionCam_Top->setCheckable(true);
        actionCam_Left = new QAction(Main_Window_Viewer);
        actionCam_Left->setObjectName(QString::fromUtf8("actionCam_Left"));
        actionCam_Left->setCheckable(true);
        actionCam_Right = new QAction(Main_Window_Viewer);
        actionCam_Right->setObjectName(QString::fromUtf8("actionCam_Right"));
        actionCam_Right->setCheckable(true);
        actionCam_Back = new QAction(Main_Window_Viewer);
        actionCam_Back->setObjectName(QString::fromUtf8("actionCam_Back"));
        actionCam_Back->setCheckable(true);
        actionCam_Model = new QAction(Main_Window_Viewer);
        actionCam_Model->setObjectName(QString::fromUtf8("actionCam_Model"));
        actionCam_Model->setCheckable(true);
        actionSave_Masks = new QAction(Main_Window_Viewer);
        actionSave_Masks->setObjectName(QString::fromUtf8("actionSave_Masks"));
        actionShow_Doodads_2 = new QAction(Main_Window_Viewer);
        actionShow_Doodads_2->setObjectName(QString::fromUtf8("actionShow_Doodads_2"));
        actionShow_Doodads_2->setCheckable(true);
        actionShow_Doodads_2->setChecked(true);
        actionLoad_Lighting = new QAction(Main_Window_Viewer);
        actionLoad_Lighting->setObjectName(QString::fromUtf8("actionLoad_Lighting"));
        actionSave_Lighting = new QAction(Main_Window_Viewer);
        actionSave_Lighting->setObjectName(QString::fromUtf8("actionSave_Lighting"));
        actionRender_Light_Objects = new QAction(Main_Window_Viewer);
        actionRender_Light_Objects->setObjectName(QString::fromUtf8("actionRender_Light_Objects"));
        actionRender_Light_Objects->setCheckable(true);
        actionUse_True_Lighting = new QAction(Main_Window_Viewer);
        actionUse_True_Lighting->setObjectName(QString::fromUtf8("actionUse_True_Lighting"));
        actionUse_True_Lighting->setCheckable(true);
        actionLightType_Dynamic = new QAction(Main_Window_Viewer);
        actionLightType_Dynamic->setObjectName(QString::fromUtf8("actionLightType_Dynamic"));
        actionLightType_Dynamic->setCheckable(true);
        actionLightType_Dynamic->setChecked(true);
        actionLightType_Ambient = new QAction(Main_Window_Viewer);
        actionLightType_Ambient->setObjectName(QString::fromUtf8("actionLightType_Ambient"));
        actionLightType_Ambient->setCheckable(true);
        actionLightType_ModelOnly = new QAction(Main_Window_Viewer);
        actionLightType_ModelOnly->setObjectName(QString::fromUtf8("actionLightType_ModelOnly"));
        actionLightType_ModelOnly->setCheckable(true);
        actionCanvasSize11_0120x0120 = new QAction(Main_Window_Viewer);
        actionCanvasSize11_0120x0120->setObjectName(QString::fromUtf8("actionCanvasSize11_0120x0120"));
        actionCanvasSize11_0120x0120->setCheckable(true);
        actionCanvasSize11_0256x0256 = new QAction(Main_Window_Viewer);
        actionCanvasSize11_0256x0256->setObjectName(QString::fromUtf8("actionCanvasSize11_0256x0256"));
        actionCanvasSize11_0256x0256->setCheckable(true);
        actionCanvasSize11_0512x0512 = new QAction(Main_Window_Viewer);
        actionCanvasSize11_0512x0512->setObjectName(QString::fromUtf8("actionCanvasSize11_0512x0512"));
        actionCanvasSize11_0512x0512->setCheckable(true);
        actionCanvasSize11_1024x1024 = new QAction(Main_Window_Viewer);
        actionCanvasSize11_1024x1024->setObjectName(QString::fromUtf8("actionCanvasSize11_1024x1024"));
        actionCanvasSize11_1024x1024->setCheckable(true);
        actionCanvasSize43_0640x0480 = new QAction(Main_Window_Viewer);
        actionCanvasSize43_0640x0480->setObjectName(QString::fromUtf8("actionCanvasSize43_0640x0480"));
        actionCanvasSize43_0640x0480->setCheckable(true);
        actionCanvasSize43_0800x0600 = new QAction(Main_Window_Viewer);
        actionCanvasSize43_0800x0600->setObjectName(QString::fromUtf8("actionCanvasSize43_0800x0600"));
        actionCanvasSize43_0800x0600->setCheckable(true);
        actionCanvasSize43_1024x0768 = new QAction(Main_Window_Viewer);
        actionCanvasSize43_1024x0768->setObjectName(QString::fromUtf8("actionCanvasSize43_1024x0768"));
        actionCanvasSize43_1024x0768->setCheckable(true);
        actionCanvasSize43_1024x0768->setChecked(true);
        actionCanvasSize43_1280x1024 = new QAction(Main_Window_Viewer);
        actionCanvasSize43_1280x1024->setObjectName(QString::fromUtf8("actionCanvasSize43_1280x1024"));
        actionCanvasSize43_1280x1024->setCheckable(true);
        actionCanvasSize43_1600x1200 = new QAction(Main_Window_Viewer);
        actionCanvasSize43_1600x1200->setObjectName(QString::fromUtf8("actionCanvasSize43_1600x1200"));
        actionCanvasSize43_1600x1200->setCheckable(true);
        actionCanvasSize169_0864x0480 = new QAction(Main_Window_Viewer);
        actionCanvasSize169_0864x0480->setObjectName(QString::fromUtf8("actionCanvasSize169_0864x0480"));
        actionCanvasSize169_0864x0480->setCheckable(true);
        actionCanvasSize169_1280x0720 = new QAction(Main_Window_Viewer);
        actionCanvasSize169_1280x0720->setObjectName(QString::fromUtf8("actionCanvasSize169_1280x0720"));
        actionCanvasSize169_1280x0720->setCheckable(true);
        actionCanvasSize169_1920x1080 = new QAction(Main_Window_Viewer);
        actionCanvasSize169_1920x1080->setObjectName(QString::fromUtf8("actionCanvasSize169_1920x1080"));
        actionCanvasSize169_1920x1080->setCheckable(true);
        actionCanvasSizeMisc_1280x0768 = new QAction(Main_Window_Viewer);
        actionCanvasSizeMisc_1280x0768->setObjectName(QString::fromUtf8("actionCanvasSizeMisc_1280x0768"));
        actionCanvasSizeMisc_1280x0768->setCheckable(true);
        actionCanvasSizeMisc_1920x1200 = new QAction(Main_Window_Viewer);
        actionCanvasSizeMisc_1920x1200->setObjectName(QString::fromUtf8("actionCanvasSizeMisc_1920x1200"));
        actionCanvasSizeMisc_1920x1200->setCheckable(true);
        actionApply_Enchants = new QAction(Main_Window_Viewer);
        actionApply_Enchants->setObjectName(QString::fromUtf8("actionApply_Enchants"));
        actionLeft_Hand_Item = new QAction(Main_Window_Viewer);
        actionLeft_Hand_Item->setObjectName(QString::fromUtf8("actionLeft_Hand_Item"));
        actionRight_Hand_Item = new QAction(Main_Window_Viewer);
        actionRight_Hand_Item->setObjectName(QString::fromUtf8("actionRight_Hand_Item"));
        actionApply_Enchants_2 = new QAction(Main_Window_Viewer);
        actionApply_Enchants_2->setObjectName(QString::fromUtf8("actionApply_Enchants_2"));
        action_List_of_Doodad_Sets = new QAction(Main_Window_Viewer);
        action_List_of_Doodad_Sets->setObjectName(QString::fromUtf8("action_List_of_Doodad_Sets"));
        actionCam_Additional = new QAction(Main_Window_Viewer);
        actionCam_Additional->setObjectName(QString::fromUtf8("actionCam_Additional"));
        actionCam_Additional->setCheckable(true);
        actionImage_ShowAlpha = new QAction(Main_Window_Viewer);
        actionImage_ShowAlpha->setObjectName(QString::fromUtf8("actionImage_ShowAlpha"));
        actionImage_ShowAlpha->setCheckable(true);
        actionDisplayImageInfo = new QAction(Main_Window_Viewer);
        actionDisplayImageInfo->setObjectName(QString::fromUtf8("actionDisplayImageInfo"));
        centralWidget = new QWidget(Main_Window_Viewer);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 10, 147, 180));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        rBtn_IsChar = new QRadioButton(layoutWidget);
        rBtn_IsChar->setObjectName(QString::fromUtf8("rBtn_IsChar"));

        gridLayout->addWidget(rBtn_IsChar, 1, 0, 1, 1);

        rBtn_IsNPC = new QRadioButton(layoutWidget);
        rBtn_IsNPC->setObjectName(QString::fromUtf8("rBtn_IsNPC"));

        gridLayout->addWidget(rBtn_IsNPC, 2, 0, 1, 1);

        rBtn_IsWMO = new QRadioButton(layoutWidget);
        rBtn_IsWMO->setObjectName(QString::fromUtf8("rBtn_IsWMO"));

        gridLayout->addWidget(rBtn_IsWMO, 4, 0, 1, 1);

        rBtn_IsADT = new QRadioButton(layoutWidget);
        rBtn_IsADT->setObjectName(QString::fromUtf8("rBtn_IsADT"));

        gridLayout->addWidget(rBtn_IsADT, 5, 0, 1, 1);

        rBtn_IsTexture = new QRadioButton(layoutWidget);
        rBtn_IsTexture->setObjectName(QString::fromUtf8("rBtn_IsTexture"));

        gridLayout->addWidget(rBtn_IsTexture, 6, 0, 1, 1);

        rBtn_NoModel = new QRadioButton(layoutWidget);
        rBtn_NoModel->setObjectName(QString::fromUtf8("rBtn_NoModel"));
        rBtn_NoModel->setChecked(true);

        gridLayout->addWidget(rBtn_NoModel, 0, 0, 1, 1);

        rBtn_IsSound = new QRadioButton(layoutWidget);
        rBtn_IsSound->setObjectName(QString::fromUtf8("rBtn_IsSound"));

        gridLayout->addWidget(rBtn_IsSound, 7, 0, 1, 1);

        rBtn_IsItem = new QRadioButton(layoutWidget);
        rBtn_IsItem->setObjectName(QString::fromUtf8("rBtn_IsItem"));

        gridLayout->addWidget(rBtn_IsItem, 3, 0, 1, 1);

        Main_Window_Viewer->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(Main_Window_Viewer);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1024, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuExport_Models = new QMenu(menuFile);
        menuExport_Models->setObjectName(QString::fromUtf8("menuExport_Models"));
        menuExport_Models->setTearOffEnabled(true);
        menuCharacter = new QMenu(menuBar);
        menuCharacter->setObjectName(QString::fromUtf8("menuCharacter"));
        menuEye_Glow = new QMenu(menuCharacter);
        menuEye_Glow->setObjectName(QString::fromUtf8("menuEye_Glow"));
        menuEye_Glow->setTearOffEnabled(true);
        menuLighting = new QMenu(menuBar);
        menuLighting->setObjectName(QString::fromUtf8("menuLighting"));
        menuUse_Lights = new QMenu(menuLighting);
        menuUse_Lights->setObjectName(QString::fromUtf8("menuUse_Lights"));
        menuOptions = new QMenu(menuBar);
        menuOptions->setObjectName(QString::fromUtf8("menuOptions"));
        menuAbout = new QMenu(menuBar);
        menuAbout->setObjectName(QString::fromUtf8("menuAbout"));
        menuSets = new QMenu(menuBar);
        menuSets->setObjectName(QString::fromUtf8("menuSets"));
        menuDoodad_Set = new QMenu(menuSets);
        menuDoodad_Set->setObjectName(QString::fromUtf8("menuDoodad_Set"));
        menuDoodad_Set->setTearOffEnabled(true);
        menuLandscape = new QMenu(menuBar);
        menuLandscape->setObjectName(QString::fromUtf8("menuLandscape"));
        menuADT_Quality = new QMenu(menuLandscape);
        menuADT_Quality->setObjectName(QString::fromUtf8("menuADT_Quality"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        menuShow_Controls = new QMenu(menuView);
        menuShow_Controls->setObjectName(QString::fromUtf8("menuShow_Controls"));
        menuShow_Controls->setTearOffEnabled(true);
        menuCamera = new QMenu(menuView);
        menuCamera->setObjectName(QString::fromUtf8("menuCamera"));
        menuCamera->setTearOffEnabled(true);
        menuSet_Canvas_Size = new QMenu(menuView);
        menuSet_Canvas_Size->setObjectName(QString::fromUtf8("menuSet_Canvas_Size"));
        menuSet_Canvas_Size->setTearOffEnabled(true);
        menuNPC = new QMenu(menuBar);
        menuNPC->setObjectName(QString::fromUtf8("menuNPC"));
        menuImage = new QMenu(menuBar);
        menuImage->setObjectName(QString::fromUtf8("menuImage"));
        Main_Window_Viewer->setMenuBar(menuBar);
        statusBar = new QStatusBar(Main_Window_Viewer);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        statusBar->setAutoFillBackground(true);
        statusBar->setSizeGripEnabled(false);
        Main_Window_Viewer->setStatusBar(statusBar);
        FileListDockWidget = new QDockWidget(Main_Window_Viewer);
        FileListDockWidget->setObjectName(QString::fromUtf8("FileListDockWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(FileListDockWidget->sizePolicy().hasHeightForWidth());
        FileListDockWidget->setSizePolicy(sizePolicy);
        FileListDockWidget->setMinimumSize(QSize(201, 201));
        FileListDockWidget->setBaseSize(QSize(201, 727));
        FileListDockWidget->setFloating(false);
        FileListDockWidget->setFeatures(QDockWidget::AllDockWidgetFeatures);
        FileListDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        FileListDockWidgetContents = new QWidget();
        FileListDockWidgetContents->setObjectName(QString::fromUtf8("FileListDockWidgetContents"));
        WoWFileTree = new QTreeWidget(FileListDockWidgetContents);
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        font.setStrikeOut(false);
        font.setKerning(true);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setFont(0, font);
        WoWFileTree->setHeaderItem(__qtreewidgetitem);
        QTreeWidgetItem *__qtreewidgetitem1 = new QTreeWidgetItem(WoWFileTree);
        QTreeWidgetItem *__qtreewidgetitem2 = new QTreeWidgetItem(__qtreewidgetitem1);
        QTreeWidgetItem *__qtreewidgetitem3 = new QTreeWidgetItem(__qtreewidgetitem2);
        new QTreeWidgetItem(__qtreewidgetitem3);
        QTreeWidgetItem *__qtreewidgetitem4 = new QTreeWidgetItem(__qtreewidgetitem2);
        new QTreeWidgetItem(__qtreewidgetitem4);
        QTreeWidgetItem *__qtreewidgetitem5 = new QTreeWidgetItem(__qtreewidgetitem1);
        QTreeWidgetItem *__qtreewidgetitem6 = new QTreeWidgetItem(__qtreewidgetitem5);
        new QTreeWidgetItem(__qtreewidgetitem6);
        QTreeWidgetItem *__qtreewidgetitem7 = new QTreeWidgetItem(__qtreewidgetitem5);
        new QTreeWidgetItem(__qtreewidgetitem7);
        QTreeWidgetItem *__qtreewidgetitem8 = new QTreeWidgetItem(WoWFileTree);
        QTreeWidgetItem *__qtreewidgetitem9 = new QTreeWidgetItem(__qtreewidgetitem8);
        new QTreeWidgetItem(__qtreewidgetitem9);
        new QTreeWidgetItem(__qtreewidgetitem9);
        QTreeWidgetItem *__qtreewidgetitem10 = new QTreeWidgetItem(WoWFileTree);
        QTreeWidgetItem *__qtreewidgetitem11 = new QTreeWidgetItem(__qtreewidgetitem10);
        new QTreeWidgetItem(__qtreewidgetitem11);
        WoWFileTree->setObjectName(QString::fromUtf8("WoWFileTree"));
        WoWFileTree->setGeometry(QRect(0, 48, 201, 611));
        WoWFileTree->setMinimumSize(QSize(101, 101));
        WoWFileTree->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);\n"
"alternate-background-color: rgb(229, 228, 242);"));
        WoWFileTree->setFrameShadow(QFrame::Sunken);
        WoWFileTree->setLineWidth(1);
        WoWFileTree->setEditTriggers(QAbstractItemView::CurrentChanged);
        WoWFileTree->setProperty("showDropIndicator", QVariant(true));
        WoWFileTree->setAlternatingRowColors(true);
        WoWFileTree->setSelectionBehavior(QAbstractItemView::SelectRows);
        WoWFileTree->setRootIsDecorated(true);
        WoWFileTree->setItemsExpandable(true);
        WoWFileTree->setAnimated(false);
        WoWFileTree->setHeaderHidden(true);
        FileTypeSelector = new QComboBox(FileListDockWidgetContents);
        FileTypeSelector->setObjectName(QString::fromUtf8("FileTypeSelector"));
        FileTypeSelector->setGeometry(QRect(0, 0, 201, 22));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(FileTypeSelector->sizePolicy().hasHeightForWidth());
        FileTypeSelector->setSizePolicy(sizePolicy1);
        FileTypeSelector->setMinimumSize(QSize(101, 0));
        FilterEditBox = new QLineEdit(FileListDockWidgetContents);
        FilterEditBox->setObjectName(QString::fromUtf8("FilterEditBox"));
        FilterEditBox->setGeometry(QRect(27, 25, 131, 20));
        sizePolicy1.setHeightForWidth(FilterEditBox->sizePolicy().hasHeightForWidth());
        FilterEditBox->setSizePolicy(sizePolicy1);
        FilterEditBox->setMinimumSize(QSize(0, 20));
        FilterEditBox->setMaximumSize(QSize(16777215, 20));
        FilterEditBox->setBaseSize(QSize(0, 20));
        FilterEditBox->setContextMenuPolicy(Qt::DefaultContextMenu);
        FilterEditBox->setAcceptDrops(false);
        FilterEditBox->setAutoFillBackground(false);
        Filterlabel = new QLabel(FileListDockWidgetContents);
        Filterlabel->setObjectName(QString::fromUtf8("Filterlabel"));
        Filterlabel->setGeometry(QRect(3, 26, 31, 16));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(Filterlabel->sizePolicy().hasHeightForWidth());
        Filterlabel->setSizePolicy(sizePolicy2);
        Filterlabel->setMaximumSize(QSize(31, 16));
        ClearFilterButton = new QPushButton(FileListDockWidgetContents);
        ClearFilterButton->setObjectName(QString::fromUtf8("ClearFilterButton"));
        ClearFilterButton->setGeometry(QRect(160, 25, 40, 20));
        sizePolicy2.setHeightForWidth(ClearFilterButton->sizePolicy().hasHeightForWidth());
        ClearFilterButton->setSizePolicy(sizePolicy2);
        ClearFilterButton->setMinimumSize(QSize(40, 20));
        ClearFilterButton->setMaximumSize(QSize(40, 20));
        ClearFilterButton->setBaseSize(QSize(40, 20));
        FileListDockWidget->setWidget(FileListDockWidgetContents);
        Main_Window_Viewer->addDockWidget(static_cast<Qt::DockWidgetArea>(1), FileListDockWidget);
#ifndef QT_NO_SHORTCUT
        Filterlabel->setBuddy(FilterEditBox);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(FileTypeSelector, FilterEditBox);
        QWidget::setTabOrder(FilterEditBox, ClearFilterButton);
        QWidget::setTabOrder(ClearFilterButton, WoWFileTree);
        QWidget::setTabOrder(WoWFileTree, rBtn_NoModel);
        QWidget::setTabOrder(rBtn_NoModel, rBtn_IsChar);
        QWidget::setTabOrder(rBtn_IsChar, rBtn_IsNPC);
        QWidget::setTabOrder(rBtn_IsNPC, rBtn_IsWMO);
        QWidget::setTabOrder(rBtn_IsWMO, rBtn_IsADT);
        QWidget::setTabOrder(rBtn_IsADT, rBtn_IsTexture);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuCharacter->menuAction());
        menuBar->addAction(menuNPC->menuAction());
        menuBar->addAction(menuSets->menuAction());
        menuBar->addAction(menuLandscape->menuAction());
        menuBar->addAction(menuImage->menuAction());
        menuBar->addAction(menuLighting->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuOptions->menuAction());
        menuBar->addAction(menuAbout->menuAction());
        menuFile->addAction(actionLoad_World_of_Wacraft);
        menuFile->addSeparator();
        menuFile->addAction(actionLoad_Character);
        menuFile->addAction(actionRandomize_Character);
        menuFile->addAction(actionLoad_NPC);
        menuFile->addSeparator();
        menuFile->addAction(actionSave_File);
        menuFile->addAction(actionSave_Screenshot);
        menuFile->addAction(actionSave_Image_Sequence);
        menuFile->addSeparator();
        menuFile->addAction(menuExport_Models->menuAction());
        menuFile->addAction(actionExport_Textures);
        menuFile->addSeparator();
        menuFile->addAction(actionDiscover_Items);
        menuFile->addAction(actionDiscover_NPCs);
        menuFile->addSeparator();
        menuFile->addAction(actionView_Log);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuExport_Models->addAction(actionExporterSettings);
        menuExport_Models->addSeparator();
        menuExport_Models->addAction(actionInitial_Pose_Only);
        menuExport_Models->addSeparator();
        menuExport_Models->addAction(actionExport_3DSMAX);
        menuExport_Models->addAction(actionExport_FBX);
        menuExport_Models->addAction(actionExport_Collada);
        menuExport_Models->addAction(actionExport_Lightwave);
        menuExport_Models->addAction(actionExport_Milkshape);
        menuExport_Models->addAction(actionExport_Ogre_XML);
        menuExport_Models->addAction(actionExport_M3);
        menuExport_Models->addAction(actionExport_OBJ);
        menuExport_Models->addAction(actionExport_X3D);
        menuExport_Models->addAction(actionExport_X3D_in_XHTML);
        menuCharacter->addAction(actionSave_Character);
        menuCharacter->addAction(actionSave_to_Model_Bank);
        menuCharacter->addSeparator();
        menuCharacter->addAction(actionClear_Equiptment);
        menuCharacter->addAction(actionLoad_Equipment);
        menuCharacter->addAction(actionLoad_Equipment_Set);
        menuCharacter->addAction(actionLoad_Starting_Outfit);
        menuCharacter->addAction(actionLoad_NPC_Outfit);
        menuCharacter->addAction(actionSave_Equipment);
        menuCharacter->addSeparator();
        menuCharacter->addAction(menuEye_Glow->menuAction());
        menuCharacter->addAction(actionShow_Underwear);
        menuCharacter->addAction(actionShow_Ears);
        menuCharacter->addAction(actionShow_Hair);
        menuCharacter->addAction(actionShow_Facial_Hair);
        menuCharacter->addAction(actionShow_Feet);
        menuCharacter->addAction(actionSheath_Weapons);
        menuCharacter->addSeparator();
        menuCharacter->addAction(actionApply_Enchants);
        menuCharacter->addSeparator();
        menuCharacter->addAction(actionLoad_a_Mount);
        menuEye_Glow->addAction(actionGlowDefault);
        menuEye_Glow->addAction(actionGlowDeathKnight);
        menuEye_Glow->addAction(actionGlowNone);
        menuLighting->addAction(actionAdjust_Lighting);
        menuLighting->addSeparator();
        menuLighting->addAction(actionLoad_Lighting);
        menuLighting->addAction(actionSave_Lighting);
        menuLighting->addSeparator();
        menuLighting->addAction(actionRender_Light_Objects);
        menuLighting->addSeparator();
        menuLighting->addAction(actionUse_True_Lighting);
        menuLighting->addAction(menuUse_Lights->menuAction());
        menuUse_Lights->addAction(actionLightType_Dynamic);
        menuUse_Lights->addAction(actionLightType_Ambient);
        menuUse_Lights->addAction(actionLightType_ModelOnly);
        menuOptions->addAction(actionSettings);
        menuOptions->addSeparator();
        menuOptions->addAction(actionExporter_Settings_2);
        menuOptions->addAction(actionInitial_Pose_Only_2);
        menuOptions->addSeparator();
        menuOptions->addAction(actionUpdate_Databases);
        menuOptions->addAction(actionUpdate_Program);
        menuAbout->addAction(actionHelp);
        menuAbout->addSeparator();
        menuAbout->addAction(actionAbout);
        menuSets->addAction(actionAlways_show_Default_Doodads);
        menuSets->addSeparator();
        menuSets->addAction(actionShow_Doodads_2);
        menuSets->addAction(menuDoodad_Set->menuAction());
        menuDoodad_Set->addAction(action_List_of_Doodad_Sets);
        menuLandscape->addAction(menuADT_Quality->menuAction());
        menuLandscape->addSeparator();
        menuLandscape->addAction(actionShow_WMOs);
        menuLandscape->addAction(actionShow_Doodads);
        menuLandscape->addSeparator();
        menuLandscape->addAction(actionSave_Masks);
        menuADT_Quality->addAction(actionADTQualityHigh);
        menuADT_Quality->addAction(actionADTQualityLow);
        menuView->addAction(menuShow_Controls->menuAction());
        menuView->addSeparator();
        menuView->addAction(menuCamera->menuAction());
        menuView->addSeparator();
        menuView->addAction(actionModel_Bank);
        menuView->addSeparator();
        menuView->addAction(actionBackground_Color);
        menuView->addAction(actionBackground_Image);
        menuView->addAction(actionSkybox);
        menuView->addSeparator();
        menuView->addAction(actionShow_Grid);
        menuView->addAction(actionShow_Mask);
        menuView->addSeparator();
        menuView->addAction(menuSet_Canvas_Size->menuAction());
        menuShow_Controls->addAction(actionShowCtrl_FileList);
        menuShow_Controls->addAction(actionShowCtrl_Animation);
        menuShow_Controls->addAction(actionShowCtrl_Character);
        menuShow_Controls->addAction(actionShowCtrl_Lighting);
        menuShow_Controls->addAction(actionShowCtrl_Model);
        menuCamera->addAction(actionCam_Model);
        menuCamera->addAction(actionCam_Additional);
        menuCamera->addSeparator();
        menuCamera->addAction(actionCam_Perspective);
        menuCamera->addSeparator();
        menuCamera->addAction(actionCam_Top);
        menuCamera->addAction(actionCam_Left);
        menuCamera->addAction(actionCam_Right);
        menuCamera->addAction(actionCam_Back);
        menuSet_Canvas_Size->addAction(actionCanvasSize11_0120x0120);
        menuSet_Canvas_Size->addAction(actionCanvasSize11_0256x0256);
        menuSet_Canvas_Size->addAction(actionCanvasSize11_0512x0512);
        menuSet_Canvas_Size->addAction(actionCanvasSize11_1024x1024);
        menuSet_Canvas_Size->addSeparator();
        menuSet_Canvas_Size->addAction(actionCanvasSize43_0640x0480);
        menuSet_Canvas_Size->addAction(actionCanvasSize43_0800x0600);
        menuSet_Canvas_Size->addAction(actionCanvasSize43_1024x0768);
        menuSet_Canvas_Size->addAction(actionCanvasSize43_1280x1024);
        menuSet_Canvas_Size->addAction(actionCanvasSize43_1600x1200);
        menuSet_Canvas_Size->addSeparator();
        menuSet_Canvas_Size->addAction(actionCanvasSize169_0864x0480);
        menuSet_Canvas_Size->addAction(actionCanvasSize169_1280x0720);
        menuSet_Canvas_Size->addAction(actionCanvasSize169_1920x1080);
        menuSet_Canvas_Size->addSeparator();
        menuSet_Canvas_Size->addAction(actionCanvasSizeMisc_1280x0768);
        menuSet_Canvas_Size->addAction(actionCanvasSizeMisc_1920x1200);
        menuNPC->addAction(actionLeft_Hand_Item);
        menuNPC->addAction(actionRight_Hand_Item);
        menuNPC->addAction(actionApply_Enchants_2);
        menuImage->addAction(actionDisplayImageInfo);
        menuImage->addSeparator();
        menuImage->addAction(actionImage_ShowAlpha);

        retranslateUi(Main_Window_Viewer);
        QObject::connect(ClearFilterButton, SIGNAL(clicked()), FilterEditBox, SLOT(clear()));

        QMetaObject::connectSlotsByName(Main_Window_Viewer);
    } // setupUi

    void retranslateUi(QMainWindow *Main_Window_Viewer)
    {
        actionLoad_World_of_Wacraft->setText(QApplication::translate("Main_Window_Viewer", "Load &World of Warcraft Data", 0, QApplication::UnicodeUTF8));
        actionView_Log->setText(QApplication::translate("Main_Window_Viewer", "&View Log...", 0, QApplication::UnicodeUTF8));
        actionSave_File->setText(QApplication::translate("Main_Window_Viewer", "Save &File...", 0, QApplication::UnicodeUTF8));
        actionSave_File->setShortcut(QApplication::translate("Main_Window_Viewer", "Ctrl+S", 0, QApplication::UnicodeUTF8));
        actionSave_Screenshot->setText(QApplication::translate("Main_Window_Viewer", "Save &Screenshot...", 0, QApplication::UnicodeUTF8));
        actionSave_Image_Sequence->setText(QApplication::translate("Main_Window_Viewer", "Save &Image Sequence...", 0, QApplication::UnicodeUTF8));
        actionExport_Textures->setText(QApplication::translate("Main_Window_Viewer", "Export &Textures...", 0, QApplication::UnicodeUTF8));
        actionInitial_Pose_Only->setText(QApplication::translate("Main_Window_Viewer", "Initial Pose Only", 0, QApplication::UnicodeUTF8));
        actionInitial_Pose_Only->setShortcut(QApplication::translate("Main_Window_Viewer", "Ctrl+I", 0, QApplication::UnicodeUTF8));
        actionExporterSettings->setText(QApplication::translate("Main_Window_Viewer", "Exporter Settings...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionExporterSettings->setToolTip(QApplication::translate("Main_Window_Viewer", "Exporter Settings", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionExport_3DSMAX->setText(QApplication::translate("Main_Window_Viewer", "3D Studio MAX...", 0, QApplication::UnicodeUTF8));
        actionExport_Collada->setText(QApplication::translate("Main_Window_Viewer", "Collada...", 0, QApplication::UnicodeUTF8));
        actionExport_FBX->setText(QApplication::translate("Main_Window_Viewer", "Autodesk FBX...", 0, QApplication::UnicodeUTF8));
        actionExport_Lightwave->setText(QApplication::translate("Main_Window_Viewer", "Lightwave 3D...", 0, QApplication::UnicodeUTF8));
        actionExport_Milkshape->setText(QApplication::translate("Main_Window_Viewer", "Milkshape...", 0, QApplication::UnicodeUTF8));
        actionExport_OBJ->setText(QApplication::translate("Main_Window_Viewer", "Wavefront OBJ...", 0, QApplication::UnicodeUTF8));
        actionExport_M3->setText(QApplication::translate("Main_Window_Viewer", "Starcraft II M3...", 0, QApplication::UnicodeUTF8));
        actionExport_Ogre_XML->setText(QApplication::translate("Main_Window_Viewer", "Ogre XML...", 0, QApplication::UnicodeUTF8));
        actionExport_X3D->setText(QApplication::translate("Main_Window_Viewer", "X3D...", 0, QApplication::UnicodeUTF8));
        actionExport_X3D_in_XHTML->setText(QApplication::translate("Main_Window_Viewer", "X3D in XHTML...", 0, QApplication::UnicodeUTF8));
        actionDiscover_Items->setText(QApplication::translate("Main_Window_Viewer", "&Discover Items", 0, QApplication::UnicodeUTF8));
        actionDiscover_NPCs->setText(QApplication::translate("Main_Window_Viewer", "Discover N&PCs", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("Main_Window_Viewer", "E&xit", 0, QApplication::UnicodeUTF8));
        actionExit->setShortcut(QApplication::translate("Main_Window_Viewer", "Ctrl+X", 0, QApplication::UnicodeUTF8));
        actionHelp->setText(QApplication::translate("Main_Window_Viewer", "&Help...", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("Main_Window_Viewer", "&About...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionAbout->setToolTip(QApplication::translate("Main_Window_Viewer", "About WoW Model Viewer", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionAlways_show_Default_Doodads->setText(QApplication::translate("Main_Window_Viewer", "Always show Default Doodads", 0, QApplication::UnicodeUTF8));
        actionSettings->setText(QApplication::translate("Main_Window_Viewer", "&Settings...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionSettings->setToolTip(QApplication::translate("Main_Window_Viewer", "Program Settings", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionExporter_Settings_2->setText(QApplication::translate("Main_Window_Viewer", "&Exporter Settings...", 0, QApplication::UnicodeUTF8));
        actionInitial_Pose_Only_2->setText(QApplication::translate("Main_Window_Viewer", "&Initial Pose Only", 0, QApplication::UnicodeUTF8));
        actionInitial_Pose_Only_2->setShortcut(QApplication::translate("Main_Window_Viewer", "Ctrl+I", 0, QApplication::UnicodeUTF8));
        actionUpdate_Databases->setText(QApplication::translate("Main_Window_Viewer", "Update &Databases...", 0, QApplication::UnicodeUTF8));
        actionUpdate_Databases->setShortcut(QApplication::translate("Main_Window_Viewer", "Ctrl+U", 0, QApplication::UnicodeUTF8));
        actionUpdate_Program->setText(QApplication::translate("Main_Window_Viewer", "Update &Program...", 0, QApplication::UnicodeUTF8));
        actionUpdate_Program->setShortcut(QApplication::translate("Main_Window_Viewer", "Ctrl+P", 0, QApplication::UnicodeUTF8));
        actionShow_WMOs->setText(QApplication::translate("Main_Window_Viewer", "Show Sets", 0, QApplication::UnicodeUTF8));
        actionShow_Doodads->setText(QApplication::translate("Main_Window_Viewer", "Show Doodads", 0, QApplication::UnicodeUTF8));
        actionADTQualityHigh->setText(QApplication::translate("Main_Window_Viewer", "High", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionADTQualityHigh->setToolTip(QApplication::translate("Main_Window_Viewer", "Quality High", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionADTQualityLow->setText(QApplication::translate("Main_Window_Viewer", "Low", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionADTQualityLow->setToolTip(QApplication::translate("Main_Window_Viewer", "Quality Low", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionLoad_Character->setText(QApplication::translate("Main_Window_Viewer", "&Load Character...", 0, QApplication::UnicodeUTF8));
        actionLoad_Character->setShortcut(QApplication::translate("Main_Window_Viewer", "F5", 0, QApplication::UnicodeUTF8));
        actionSave_Character->setText(QApplication::translate("Main_Window_Viewer", "Save Character...", 0, QApplication::UnicodeUTF8));
        actionSave_Character->setShortcut(QApplication::translate("Main_Window_Viewer", "F6", 0, QApplication::UnicodeUTF8));
        actionRandomize_Character->setText(QApplication::translate("Main_Window_Viewer", "Import &Armory Character...", 0, QApplication::UnicodeUTF8));
        actionLoad_Equipment->setText(QApplication::translate("Main_Window_Viewer", "Load Equipment...", 0, QApplication::UnicodeUTF8));
        actionLoad_Equipment->setShortcut(QApplication::translate("Main_Window_Viewer", "F7", 0, QApplication::UnicodeUTF8));
        actionSave_Equipment->setText(QApplication::translate("Main_Window_Viewer", "Save Equipment...", 0, QApplication::UnicodeUTF8));
        actionSave_Equipment->setShortcut(QApplication::translate("Main_Window_Viewer", "F8", 0, QApplication::UnicodeUTF8));
        actionClear_Equiptment->setText(QApplication::translate("Main_Window_Viewer", "Clear Equipment", 0, QApplication::UnicodeUTF8));
        actionClear_Equiptment->setShortcut(QApplication::translate("Main_Window_Viewer", "F9", 0, QApplication::UnicodeUTF8));
        actionLoad_Equipment_Set->setText(QApplication::translate("Main_Window_Viewer", "Load Equipment Set...", 0, QApplication::UnicodeUTF8));
        actionLoad_Starting_Outfit->setText(QApplication::translate("Main_Window_Viewer", "Load Starting Outfit...", 0, QApplication::UnicodeUTF8));
        actionLoad_NPC_Outfit->setText(QApplication::translate("Main_Window_Viewer", "Load NPC Outfit...", 0, QApplication::UnicodeUTF8));
        actionGlowNone->setText(QApplication::translate("Main_Window_Viewer", "None", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionGlowNone->setToolTip(QApplication::translate("Main_Window_Viewer", "No Eyeglow", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionGlowDefault->setText(QApplication::translate("Main_Window_Viewer", "Default", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionGlowDefault->setToolTip(QApplication::translate("Main_Window_Viewer", "Default Eyeglow", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionGlowDeathKnight->setText(QApplication::translate("Main_Window_Viewer", "Death Knight", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionGlowDeathKnight->setToolTip(QApplication::translate("Main_Window_Viewer", "Death Knight Eyeglow", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionShow_Underwear->setText(QApplication::translate("Main_Window_Viewer", "Show Underwear", 0, QApplication::UnicodeUTF8));
        actionShow_Ears->setText(QApplication::translate("Main_Window_Viewer", "Show Ears", 0, QApplication::UnicodeUTF8));
        actionShow_Ears->setShortcut(QApplication::translate("Main_Window_Viewer", "Ctrl+E", 0, QApplication::UnicodeUTF8));
        actionShow_Hair->setText(QApplication::translate("Main_Window_Viewer", "Show Hair", 0, QApplication::UnicodeUTF8));
        actionShow_Hair->setShortcut(QApplication::translate("Main_Window_Viewer", "Ctrl+H", 0, QApplication::UnicodeUTF8));
        actionShow_Facial_Hair->setText(QApplication::translate("Main_Window_Viewer", "Show Facial Hair", 0, QApplication::UnicodeUTF8));
        actionShow_Facial_Hair->setShortcut(QApplication::translate("Main_Window_Viewer", "Ctrl+F", 0, QApplication::UnicodeUTF8));
        actionShow_Feet->setText(QApplication::translate("Main_Window_Viewer", "Show Feet", 0, QApplication::UnicodeUTF8));
        actionSheath_Weapons->setText(QApplication::translate("Main_Window_Viewer", "Sheath Weapons", 0, QApplication::UnicodeUTF8));
        actionSheath_Weapons->setShortcut(QApplication::translate("Main_Window_Viewer", "Ctrl+Z", 0, QApplication::UnicodeUTF8));
        actionLoad_a_Mount->setText(QApplication::translate("Main_Window_Viewer", "Load a Mount...", 0, QApplication::UnicodeUTF8));
        actionLoad_NPC->setText(QApplication::translate("Main_Window_Viewer", "Load &NPC...", 0, QApplication::UnicodeUTF8));
        actionAdjust_Lighting->setText(QApplication::translate("Main_Window_Viewer", "&Edit Lighting...", 0, QApplication::UnicodeUTF8));
        actionShowCtrl_Animation->setText(QApplication::translate("Main_Window_Viewer", "Animation", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionShowCtrl_Animation->setToolTip(QApplication::translate("Main_Window_Viewer", "Show Animation Controls", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionShowCtrl_Character->setText(QApplication::translate("Main_Window_Viewer", "Character", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionShowCtrl_Character->setToolTip(QApplication::translate("Main_Window_Viewer", "Show Character Controls", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionShowCtrl_Lighting->setText(QApplication::translate("Main_Window_Viewer", "Lighting", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionShowCtrl_Lighting->setToolTip(QApplication::translate("Main_Window_Viewer", "Show Lighting Controls", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionShowCtrl_Model->setText(QApplication::translate("Main_Window_Viewer", "Model", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionShowCtrl_Model->setToolTip(QApplication::translate("Main_Window_Viewer", "Show Model Controls", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionModel_Bank->setText(QApplication::translate("Main_Window_Viewer", "Model &Bank...", 0, QApplication::UnicodeUTF8));
        actionSave_to_Model_Bank->setText(QApplication::translate("Main_Window_Viewer", "Save to Model Bank...", 0, QApplication::UnicodeUTF8));
        actionSave_to_Model_Bank->setShortcut(QApplication::translate("Main_Window_Viewer", "Alt+S", 0, QApplication::UnicodeUTF8));
        actionShowCtrl_FileList->setText(QApplication::translate("Main_Window_Viewer", "File List", 0, QApplication::UnicodeUTF8));
        actionBackground_Color->setText(QApplication::translate("Main_Window_Viewer", "Background Color...", 0, QApplication::UnicodeUTF8));
        actionBackground_Image->setText(QApplication::translate("Main_Window_Viewer", "Background Image...", 0, QApplication::UnicodeUTF8));
        actionSkybox->setText(QApplication::translate("Main_Window_Viewer", "Load Skybox...", 0, QApplication::UnicodeUTF8));
        actionShow_Grid->setText(QApplication::translate("Main_Window_Viewer", "Show Grid", 0, QApplication::UnicodeUTF8));
        actionShow_Mask->setText(QApplication::translate("Main_Window_Viewer", "Show Mask", 0, QApplication::UnicodeUTF8));
        actionCam_Perspective->setText(QApplication::translate("Main_Window_Viewer", "&Perspective", 0, QApplication::UnicodeUTF8));
        actionCam_Top->setText(QApplication::translate("Main_Window_Viewer", "&Top", 0, QApplication::UnicodeUTF8));
        actionCam_Left->setText(QApplication::translate("Main_Window_Viewer", "&Left", 0, QApplication::UnicodeUTF8));
        actionCam_Right->setText(QApplication::translate("Main_Window_Viewer", "&Right", 0, QApplication::UnicodeUTF8));
        actionCam_Back->setText(QApplication::translate("Main_Window_Viewer", "&Back", 0, QApplication::UnicodeUTF8));
        actionCam_Model->setText(QApplication::translate("Main_Window_Viewer", "&Model Camera", 0, QApplication::UnicodeUTF8));
        actionSave_Masks->setText(QApplication::translate("Main_Window_Viewer", "Save Masks...", 0, QApplication::UnicodeUTF8));
        actionShow_Doodads_2->setText(QApplication::translate("Main_Window_Viewer", "Show Doodads", 0, QApplication::UnicodeUTF8));
        actionLoad_Lighting->setText(QApplication::translate("Main_Window_Viewer", "&Load Lighting...", 0, QApplication::UnicodeUTF8));
        actionSave_Lighting->setText(QApplication::translate("Main_Window_Viewer", "&Save Lighting...", 0, QApplication::UnicodeUTF8));
        actionRender_Light_Objects->setText(QApplication::translate("Main_Window_Viewer", "&Render Light Objects", 0, QApplication::UnicodeUTF8));
        actionUse_True_Lighting->setText(QApplication::translate("Main_Window_Viewer", "Use &True Lighting", 0, QApplication::UnicodeUTF8));
        actionLightType_Dynamic->setText(QApplication::translate("Main_Window_Viewer", "&Dynamic Lights", 0, QApplication::UnicodeUTF8));
        actionLightType_Ambient->setText(QApplication::translate("Main_Window_Viewer", "&Ambient Light", 0, QApplication::UnicodeUTF8));
        actionLightType_ModelOnly->setText(QApplication::translate("Main_Window_Viewer", "&Model Lights Only", 0, QApplication::UnicodeUTF8));
        actionCanvasSize11_0120x0120->setText(QApplication::translate("Main_Window_Viewer", "120x120", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionCanvasSize11_0120x0120->setToolTip(QApplication::translate("Main_Window_Viewer", "120x120, 1:1 Ratio", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionCanvasSize11_0256x0256->setText(QApplication::translate("Main_Window_Viewer", "256x256", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionCanvasSize11_0256x0256->setToolTip(QApplication::translate("Main_Window_Viewer", "256x256, 1:1 Ratio", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionCanvasSize11_0512x0512->setText(QApplication::translate("Main_Window_Viewer", "512x512", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionCanvasSize11_0512x0512->setToolTip(QApplication::translate("Main_Window_Viewer", "512x512, 1:1 Ratio", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionCanvasSize11_1024x1024->setText(QApplication::translate("Main_Window_Viewer", "1024x1024", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionCanvasSize11_1024x1024->setToolTip(QApplication::translate("Main_Window_Viewer", "1024x1024, 1:1 Ratio", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionCanvasSize43_0640x0480->setText(QApplication::translate("Main_Window_Viewer", "640x480", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionCanvasSize43_0640x0480->setToolTip(QApplication::translate("Main_Window_Viewer", "640x480, 4:3 Ratio", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionCanvasSize43_0800x0600->setText(QApplication::translate("Main_Window_Viewer", "800x600", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionCanvasSize43_0800x0600->setToolTip(QApplication::translate("Main_Window_Viewer", "800x600, 4:3 Ratio", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionCanvasSize43_1024x0768->setText(QApplication::translate("Main_Window_Viewer", "1024x768", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionCanvasSize43_1024x0768->setToolTip(QApplication::translate("Main_Window_Viewer", "1024x768, 4:3 Ratio", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionCanvasSize43_1280x1024->setText(QApplication::translate("Main_Window_Viewer", "1280x1024", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionCanvasSize43_1280x1024->setToolTip(QApplication::translate("Main_Window_Viewer", "1280x1024, 4:3 Ratio", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionCanvasSize43_1600x1200->setText(QApplication::translate("Main_Window_Viewer", "1600x1200", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionCanvasSize43_1600x1200->setToolTip(QApplication::translate("Main_Window_Viewer", "1600x1200, 4:3 Ratio", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionCanvasSize169_0864x0480->setText(QApplication::translate("Main_Window_Viewer", "864x480", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionCanvasSize169_0864x0480->setToolTip(QApplication::translate("Main_Window_Viewer", "864x480, 16:9 Ratio", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionCanvasSize169_1280x0720->setText(QApplication::translate("Main_Window_Viewer", "1280x720", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionCanvasSize169_1280x0720->setToolTip(QApplication::translate("Main_Window_Viewer", "1280x720, 16:9 Ratio", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionCanvasSize169_1920x1080->setText(QApplication::translate("Main_Window_Viewer", "1920x1080", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionCanvasSize169_1920x1080->setToolTip(QApplication::translate("Main_Window_Viewer", "1920x1080, 16:9 Ratio", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionCanvasSizeMisc_1280x0768->setText(QApplication::translate("Main_Window_Viewer", "1280x768", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionCanvasSizeMisc_1280x0768->setToolTip(QApplication::translate("Main_Window_Viewer", "1280x768, 5:3 Ratio", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionCanvasSizeMisc_1920x1200->setText(QApplication::translate("Main_Window_Viewer", "1920x1200", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionCanvasSizeMisc_1920x1200->setToolTip(QApplication::translate("Main_Window_Viewer", "1920x1200, 8:5 Ratio", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionApply_Enchants->setText(QApplication::translate("Main_Window_Viewer", "Apply Enchants...", 0, QApplication::UnicodeUTF8));
        actionLeft_Hand_Item->setText(QApplication::translate("Main_Window_Viewer", "Left Hand Item...", 0, QApplication::UnicodeUTF8));
        actionRight_Hand_Item->setText(QApplication::translate("Main_Window_Viewer", "Right Hand Item...", 0, QApplication::UnicodeUTF8));
        actionApply_Enchants_2->setText(QApplication::translate("Main_Window_Viewer", "Apply Enchants...", 0, QApplication::UnicodeUTF8));
        action_List_of_Doodad_Sets->setText(QApplication::translate("Main_Window_Viewer", "(List of Doodad Sets)", 0, QApplication::UnicodeUTF8));
        actionCam_Additional->setText(QApplication::translate("Main_Window_Viewer", "(Additional Cameras)", 0, QApplication::UnicodeUTF8));
        actionImage_ShowAlpha->setText(QApplication::translate("Main_Window_Viewer", "Show &Alpha", 0, QApplication::UnicodeUTF8));
        actionDisplayImageInfo->setText(QApplication::translate("Main_Window_Viewer", "&Display Info", 0, QApplication::UnicodeUTF8));
        rBtn_IsChar->setText(QApplication::translate("Main_Window_Viewer", "File is Character", 0, QApplication::UnicodeUTF8));
        rBtn_IsNPC->setText(QApplication::translate("Main_Window_Viewer", "File is Creature/NPC", 0, QApplication::UnicodeUTF8));
        rBtn_IsWMO->setText(QApplication::translate("Main_Window_Viewer", "File is WMO/Set", 0, QApplication::UnicodeUTF8));
        rBtn_IsADT->setText(QApplication::translate("Main_Window_Viewer", "File is ADT/Landscape", 0, QApplication::UnicodeUTF8));
        rBtn_IsTexture->setText(QApplication::translate("Main_Window_Viewer", "File is an Image", 0, QApplication::UnicodeUTF8));
        rBtn_NoModel->setText(QApplication::translate("Main_Window_Viewer", "No File Loaded", 0, QApplication::UnicodeUTF8));
        rBtn_IsSound->setText(QApplication::translate("Main_Window_Viewer", "File is an Audio/Sound file", 0, QApplication::UnicodeUTF8));
        rBtn_IsItem->setText(QApplication::translate("Main_Window_Viewer", "File is an Item", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("Main_Window_Viewer", "&File", 0, QApplication::UnicodeUTF8));
        menuExport_Models->setTitle(QApplication::translate("Main_Window_Viewer", "&Export Models", 0, QApplication::UnicodeUTF8));
        menuCharacter->setTitle(QApplication::translate("Main_Window_Viewer", "&Character", 0, QApplication::UnicodeUTF8));
        menuEye_Glow->setTitle(QApplication::translate("Main_Window_Viewer", "Eye Glow", 0, QApplication::UnicodeUTF8));
        menuLighting->setTitle(QApplication::translate("Main_Window_Viewer", "L&ighting", 0, QApplication::UnicodeUTF8));
        menuUse_Lights->setTitle(QApplication::translate("Main_Window_Viewer", "&Use Lights", 0, QApplication::UnicodeUTF8));
        menuOptions->setTitle(QApplication::translate("Main_Window_Viewer", "Options", 0, QApplication::UnicodeUTF8));
        menuAbout->setTitle(QApplication::translate("Main_Window_Viewer", "&About", 0, QApplication::UnicodeUTF8));
        menuSets->setTitle(QApplication::translate("Main_Window_Viewer", "&Set", 0, QApplication::UnicodeUTF8));
        menuDoodad_Set->setTitle(QApplication::translate("Main_Window_Viewer", "Doodad Set", 0, QApplication::UnicodeUTF8));
        menuLandscape->setTitle(QApplication::translate("Main_Window_Viewer", "&Landscape", 0, QApplication::UnicodeUTF8));
        menuADT_Quality->setTitle(QApplication::translate("Main_Window_Viewer", "Landscape Quality", 0, QApplication::UnicodeUTF8));
        menuView->setTitle(QApplication::translate("Main_Window_Viewer", "View", 0, QApplication::UnicodeUTF8));
        menuShow_Controls->setTitle(QApplication::translate("Main_Window_Viewer", "&Show Controls", 0, QApplication::UnicodeUTF8));
        menuCamera->setTitle(QApplication::translate("Main_Window_Viewer", "&Camera", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        menuSet_Canvas_Size->setStatusTip(QApplication::translate("Main_Window_Viewer", "Change the size of the Canvas to fit your needs.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        menuSet_Canvas_Size->setTitle(QApplication::translate("Main_Window_Viewer", "Set Canvas Size", 0, QApplication::UnicodeUTF8));
        menuNPC->setTitle(QApplication::translate("Main_Window_Viewer", "Creature/NPC", 0, QApplication::UnicodeUTF8));
        menuImage->setTitle(QApplication::translate("Main_Window_Viewer", "&Image", 0, QApplication::UnicodeUTF8));
        FileListDockWidget->setWindowTitle(QApplication::translate("Main_Window_Viewer", "WoW Files", "WoWFiles", QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = WoWFileTree->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("Main_Window_Viewer", "WoW Files", 0, QApplication::UnicodeUTF8));

        const bool __sortingEnabled = WoWFileTree->isSortingEnabled();
        WoWFileTree->setSortingEnabled(false);
        QTreeWidgetItem *___qtreewidgetitem1 = WoWFileTree->topLevelItem(0);
        ___qtreewidgetitem1->setText(0, QApplication::translate("Main_Window_Viewer", "Character", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem2 = ___qtreewidgetitem1->child(0);
        ___qtreewidgetitem2->setText(0, QApplication::translate("Main_Window_Viewer", "Nightelf", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem3 = ___qtreewidgetitem2->child(0);
        ___qtreewidgetitem3->setText(0, QApplication::translate("Main_Window_Viewer", "Female", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem4 = ___qtreewidgetitem3->child(0);
        ___qtreewidgetitem4->setText(0, QApplication::translate("Main_Window_Viewer", "nightelffemale.m2", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem5 = ___qtreewidgetitem2->child(1);
        ___qtreewidgetitem5->setText(0, QApplication::translate("Main_Window_Viewer", "Male", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem6 = ___qtreewidgetitem5->child(0);
        ___qtreewidgetitem6->setText(0, QApplication::translate("Main_Window_Viewer", "nightelfmale.m2", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem7 = ___qtreewidgetitem1->child(1);
        ___qtreewidgetitem7->setText(0, QApplication::translate("Main_Window_Viewer", "Tauren", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem8 = ___qtreewidgetitem7->child(0);
        ___qtreewidgetitem8->setText(0, QApplication::translate("Main_Window_Viewer", "Female", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem9 = ___qtreewidgetitem8->child(0);
        ___qtreewidgetitem9->setText(0, QApplication::translate("Main_Window_Viewer", "taurenfemale.m2", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem10 = ___qtreewidgetitem7->child(1);
        ___qtreewidgetitem10->setText(0, QApplication::translate("Main_Window_Viewer", "Male", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem11 = ___qtreewidgetitem10->child(0);
        ___qtreewidgetitem11->setText(0, QApplication::translate("Main_Window_Viewer", "taurenmale.m2", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem12 = WoWFileTree->topLevelItem(1);
        ___qtreewidgetitem12->setText(0, QApplication::translate("Main_Window_Viewer", "Creature", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem13 = ___qtreewidgetitem12->child(0);
        ___qtreewidgetitem13->setText(0, QApplication::translate("Main_Window_Viewer", "Arthaslichking", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem14 = ___qtreewidgetitem13->child(0);
        ___qtreewidgetitem14->setText(0, QApplication::translate("Main_Window_Viewer", "arthaslichking.m2", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem15 = ___qtreewidgetitem13->child(1);
        ___qtreewidgetitem15->setText(0, QApplication::translate("Main_Window_Viewer", "arthaslichking_unarmed.m2", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem16 = WoWFileTree->topLevelItem(2);
        ___qtreewidgetitem16->setText(0, QApplication::translate("Main_Window_Viewer", "Items", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem17 = ___qtreewidgetitem16->child(0);
        ___qtreewidgetitem17->setText(0, QApplication::translate("Main_Window_Viewer", "Sword", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem18 = ___qtreewidgetitem17->child(0);
        ___qtreewidgetitem18->setText(0, QApplication::translate("Main_Window_Viewer", "sword.m2", 0, QApplication::UnicodeUTF8));
        WoWFileTree->setSortingEnabled(__sortingEnabled);

        FileTypeSelector->clear();
        FileTypeSelector->insertItems(0, QStringList()
         << QApplication::translate("Main_Window_Viewer", "Model (M2) Files", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Main_Window_Viewer", "Set (WMO) Files", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Main_Window_Viewer", "Landscape (ADT) Files", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Main_Window_Viewer", "Image (BLP) Files", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Main_Window_Viewer", "Sound (WAV, OGG, MP3) Files", 0, QApplication::UnicodeUTF8)
        );
        FilterEditBox->setInputMask(QString());
        FilterEditBox->setText(QString());
        FilterEditBox->setPlaceholderText(QString());
        Filterlabel->setText(QApplication::translate("Main_Window_Viewer", "Filter", 0, QApplication::UnicodeUTF8));
        ClearFilterButton->setText(QApplication::translate("Main_Window_Viewer", "Clear", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(Main_Window_Viewer);
    } // retranslateUi

};

namespace Ui {
    class Main_Window_Viewer: public Ui_Main_Window_Viewer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAIN_WINDOW_VIEWER_H

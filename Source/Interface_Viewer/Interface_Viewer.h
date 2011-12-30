/*
	Viewer Interface Header
*/

#ifndef INTERFACE_VIEWER_H
#define INTERFACE_VIEWER_H

#include "../Engine/kernel.h"
#include <QMainWindow>
#include <QMessageBox>
#include "../Engine/classes.h"
#include "../Application/Settings.h"
#include "../Engine/WoWDirectoryTools.h"

// UI Files
#include "GeneratedFiles/ui_Interface_Viewer.h"

// Commonly used Windows
#include "../CommonWindows/About.h"
#include "../CommonWindows/AboutPlugins.h"
#include "../CommonWindows/WoWDirManager.h"

namespace Ui {
    class Interface_Viewer;
}

namespace WMVEngine {

// Viewer Interface Window
class Interface_Viewer : public QMainWindow
{
	Q_OBJECT

public:
	explicit Interface_Viewer(QWidget *parent = 0);
	~Interface_Viewer();
	
	// Vars
	size_t InterfaceMode;
    size_t ViewerInterfaceType;
	size_t WoWTypeCurrent;
	size_t WoWTypeNext;
	st_WoWDir CurrentDir;

	// Functions
	void init();
    void UpdateViewerMenu();
	void createStatusBar();
	void updateStatusBar(QString);
	void updateFileList();
	void LoadWoW();
	void UnLoadWoW();

	// Icons
	QIcon icon01Vanilla;
	QIcon icon02TBC;
	QIcon icon03WotLK;
	QIcon icon04Cata;
	QIcon icon05MoP;
	QIcon iconPTR;
	QIcon iconDisabled;

	// Booleans
	bool isWoWLoaded;		// Is there a version of WoW loaded.
	bool canReloadWoW;		// If isWoWLoaded == true, are we allowed to reload it? Should only be used if the MPQ list is changed, or we change WoW directories.

	// Groups
	QActionGroup *WoWDirGroup;
    QActionGroup *EyeGlowGroup;
    QActionGroup *LandscapeQualityGroup;
    QActionGroup *LightTypeGroup;
    QActionGroup *CanvasSizeGroup;
	QActionGroup *ViewerModeGroup;
    QActionGroup *CameraGroup;
    QActionGroup *DoodadSetGroup;

	// SubWindows
	WindowAbout Window_About;
	WindowAboutPlugins Window_AboutPlugins;
	WoWDirManager WoWDirectoryManager;

	// User-Interface
    Ui::Interface_Viewer *ui;

public slots:
	//void ChangeLoadButton();
	//void UpdateLoadButton();

private:

private slots:
	void updateCurrentDirfromMenu();
    void on_actionInitial_Pose_Only_2_toggled(bool arg1);
    void on_actionInitial_Pose_Only_toggled(bool arg1);
	void on_actionShowCtrl_FileList_toggled(bool arg1);

    void on_rBtn_NoModel_clicked();
    void on_rBtn_IsChar_clicked();
    void on_rBtn_IsNPC_clicked();
    void on_rBtn_IsADT_clicked();
    void on_rBtn_IsWMO_clicked();
    void on_rBtn_IsTexture_clicked();
	void on_rBtn_IsItem_clicked();
    void on_rBtn_IsSound_clicked();
	
	// File Menu
	void on_actionLoad_World_of_Wacraft_triggered();
	void on_actionView_Log_triggered();
    void on_actionExit_triggered();

	// Options Menu
	void on_actionManage_Directories_triggered();
	
	// About Menu
	void on_actionAbout_triggered();
	void on_actionAbout_Plugins_triggered();
};

} // namespace

#endif // INTERFACE_VIEWER_H
#ifndef APPLICATION_H
#define APPLICATION_H

// QT Includes
#include <QMainWindow>
#include <QActionGroup>
#include <QMenu>
#include <QDate>
#include <QMessageBox>

// Be careful of what you #include here. This header is included with the auto-generated
// moc_application.cpp file, and can sometimes cause linking errors.

// Globals
#include "version.h"
#include "enums.h"
#include "lists.h"
#include "classes.h"

// Windows
#include "ui_Main_Window_Viewer.h"
#include "ui_About.h"
#include "wowdirmanager.h"

// Functions & Stormlib
#include "math\math_base.h"
#include "StormLib\src\StormLib.h"

namespace Ui {
    class Main_Window_Viewer;
	class AboutWindow;
	class WoWDirManager;
}

// About Window
class WindowAbout : public QDialog
{
	Q_OBJECT

public:
    explicit WindowAbout(QWidget *parent = 0);
	~WindowAbout();
private slots:
	void on_CloseButton_clicked() {
		QDialog::close();
	}

private:
	Ui::AboutWindow *ui_About;
};

// WoW Model Viewer
class WoWModelViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit WoWModelViewer(QWidget *parent = 0);
	size_t InterfaceMode;
    size_t ViewerInterfaceType;
	size_t WoWTypeCurrent;
	size_t WoWTypeNext;
	st_WoWDir CurrentDir;
    void UpdateViewerMenu();
	void createStatusBar();
	void updateStatusBar(QString);
	void updateFileList();
    ~WoWModelViewer();

    void init();

	// Booleans
	bool isWoWLoaded;		// Is there a version of WoW loaded.
	bool canReloadWoW;		// If isWoWLoaded == true, are we allowed to reload it? Should only be used if the MPQ list is changed, or we change WoW directories.

	// SubWindows
	WindowAbout Window_About;
	WoWDirManager WoWDirectoryManager;

public slots:
	void ChangeLoadButton();
	void UpdateLoadButton();

private:
    // Groups
	QActionGroup *WoWDirGroup;
    QActionGroup *EyeGlowGroup;
    QActionGroup *LandscapeQualityGroup;
    QActionGroup *LightTypeGroup;
    QActionGroup *CanvasSizeGroup;
	QActionGroup *ViewerModeGroup;
    QActionGroup *CameraGroup;
    QActionGroup *DoodadSetGroup;

private slots:
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
    void on_actionExit_triggered();

	// Options Menu
	void on_actionManage_Directories_triggered();

	// About Menu
	void on_actionAbout_triggered();

private:
    Ui::Main_Window_Viewer *ui;
};

#endif // MAINWINDOW_H

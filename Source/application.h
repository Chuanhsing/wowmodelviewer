#ifndef APPLICATION_H
#define APPLICATION_H

#include <QMainWindow>
#include <QActionGroup>
#include <QMenu>

// Be careful of what you #include here. This header is included with the auto-generated
// moc_application.cpp file, and can sometimes cause linking errors.
#include "enums.h"
#include "math\math_base.h"
#include "Exporters\exporters.h"
#include "StormLib\src\StormLib.h"


namespace Ui {
    class Main_Window_Viewer;
}

class WoWModelViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit WoWModelViewer(QWidget *parent = 0);
	size_t InterfaceMode;
    size_t ViewerInterfaceType;
    void UpdateViewerMenu();
	void createStatusBar();
	void updateFileList();
    ~WoWModelViewer();

	// Booleans
	bool isWoWLoaded;		// Is there a version of WoW loaded.
	bool canReloadWoW;		// If isWoWLoaded == true, are we allowed to reload it? Should only be used if the MPQ list is changed, or we change WoW directories.

private:
    // Groups
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

	void on_rBtn_WoWNotLoaded_clicked();
    void on_rBtn_NoModel_clicked();
    void on_rBtn_IsChar_clicked();
    void on_rBtn_IsNPC_clicked();
    void on_rBtn_IsADT_clicked();
    void on_rBtn_IsWMO_clicked();
    void on_rBtn_IsTexture_clicked();
	void on_rBtn_IsItem_clicked();
    void on_rBtn_IsSound_clicked();

    void on_actionExit_triggered();

private:
    Ui::Main_Window_Viewer *ui;
};

#endif // MAINWINDOW_H

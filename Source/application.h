#ifndef APPLICATION_H
#define APPLICATION_H

#include <QMainWindow>
#include <QActionGroup>
#include <QMenu>
#include "enums.h"

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

private:
    // Groups
    QActionGroup *EyeGlowGroup;
    QActionGroup *LandscapeQualityGroup;
    QActionGroup *LightTypeGroup;
    QActionGroup *CanvasSizeGroup;
    QActionGroup *CameraGroup;
    QActionGroup *DoodadSetGroup;

private slots:
    void on_actionInitial_Pose_Only_2_toggled(bool arg1);
    void on_actionInitial_Pose_Only_toggled(bool arg1);

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

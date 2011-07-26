#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QActionGroup>
#include <QMenu>
#include "enums.h"

namespace Ui {
    class MainWindow;
}

class WoWModelViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit WoWModelViewer(QWidget *parent = 0);
    size_t InterfaceType;
    void UpdateMenu();
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

    void on_actionExit_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

/*
	Cinema Interface Header
*/

#ifndef INTERFACE_CINEMA_H
#define INTERFACE_CINEMA_H

#include "../Engine/kernel.h"
#include <QMainWindow>
#include <QMessageBox>
#include "../Engine/classes.h"
#include "../Application/Settings.h"
#include "../Games/WorldofWarcraft/DirectoryTools_WoW.h"

// UI Files
#include "GeneratedFiles/ui_Interface_Cinema.h"

// Commonly used Windows
#include "../CommonWindows/About.h"
#include "../CommonWindows/AboutPlugins.h"
#include "../CommonWindows/GameDirManager.h"

namespace Ui {
    class Interface_Cinema;
}

namespace WMVEngine {

// Viewer Interface Window
class Interface_Cinema : public QMainWindow
{
	Q_OBJECT

public:
	explicit Interface_Cinema(QWidget *parent = 0);
	~Interface_Cinema();

	// SubWindows
	WindowAbout Window_About;
	WindowAboutPlugins Window_AboutPlugins;
	GameDirManager GameDirectoryManager;

	// User-Interface
    Ui::Interface_Cinema *ui;

	// Functions
	void init();
    void UpdateViewerMenu();
	void createStatusBar();
	void updateStatusBar(QString);

	// Icons
	QIcon icon01Vanilla;
	QIcon icon02TBC;
	QIcon icon03WotLK;
	QIcon icon04Cata;
	QIcon icon05MoP;
	QIcon iconPTR;
	QIcon iconDisabled;

	// Groups
	QActionGroup *ViewerModeGroup;

public slots:

private:

private slots:
	// File Menu
	void on_actionView_Log_triggered();
    void on_actionExit_triggered();
	
	// About Menu
	void on_actionAbout_triggered();
	void on_actionAbout_Plugins_triggered();
};

}

#endif // INTERFACE_CINEMA_H
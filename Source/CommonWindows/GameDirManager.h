#ifndef WOWDIRMANAGER_H
#define WOWDIRMANAGER_H

#include "../Engine/wmv_engine.h"

#include <QDir>
#include <QListWidget>
#include "GeneratedFiles/ui_GameDirManager_Main.h"
#include "GeneratedFiles/ui_GameDirManager_Edit.h"
#include "../Engine/classes.h"
#include "../Games/WorldofWarcraft/DirectoryTools_WoW.h"

class QAbstractItemModel;
class QItemSelectionModel;

namespace WMVEngine {

class GameDirManager_Edit : public QDialog
{
	Q_OBJECT

public:
	explicit GameDirManager_Edit(QWidget *parent = 0);
	~GameDirManager_Edit();
	void init();

private slots:
	void on_btn_Okay_clicked();
	void on_btn_Cancel_clicked() {
		QDialog::close();
	}

private:
	Ui::GameDirManager_Edit *ui_GameDirManager_Edit;
};

class GameDirManager : public QDialog
{
	Q_OBJECT

public:
	// Public Functions
    explicit GameDirManager(QWidget *parent = 0);
	~GameDirManager();
	void init();

private:
	void SetupList();
	void UpdateList();
	
	/*
	// Icons
	QIcon icon01Vanilla;
	QIcon icon02TBC;
	QIcon icon03WotLK;
	QIcon icon04Cata;
	QIcon icon05MoP;
	QIcon iconPTR;

	QString selected;		// The Selected WoWDir's name
	*/
	QListWidget *List;

	// SubWindows
	GameDirManager_Edit Window_GameDirManagerEdit;

	void saveDir(st_WoWDir,bool);				// Save the Directory
	void ListOffset(int);
	template <class UnReal, class Real>
	void ReOrderList(UnReal, Real);				// Re-orders the WoW list

private slots:
	void on_GameDirManager_bDirMakeCurrent_clicked();
	void on_GameDirManager_bDirAdd_clicked();
	void on_GameDirManager_bDirEdit_clicked();
	void on_GameDirManager_bDirUp_clicked();
	void on_GameDirManager_bDirDown_clicked();
	void on_GameDirManager_bDirDeleteAll_clicked();
	void on_buttonBox_Cancel_clicked() {
		QDialog::close();
	}
private:
	Ui::GameDirManager *ui_GameDirManager;
};

st_WoWDir ScanWoWDir(QDir, int, int);

} // Namespace

#endif
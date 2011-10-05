#ifndef WOWDIRMANAGER_H
#define WOWDIRMANAGER_H

#include <QDir>
#include <QListWidget>
#include "ui_WoWDirManager.h"
#include "classes.h"

class QAbstractItemModel;
class QItemSelectionModel;

class WoWDirManager : public QDialog
{
	Q_OBJECT

public:
    explicit WoWDirManager(QWidget *parent = 0);
	~WoWDirManager();

	//Functions
	void init();
	void SetupList();
	void UpdateList();

	// Icons
	QIcon iconVanilla;
	QIcon iconTBC;
	QIcon iconWotLK;
	QIcon iconCata;
	QIcon iconPTR;

	QString selected;		// The Selected WoWDir's name

	QListWidget *List;

private:
	void saveDir(st_WoWDir,bool);				// Save the Directory
	void ListOffset(int);
	template <class UnReal, class Real>
	void ReOrderList(UnReal, Real);			// Re-orders the WoW list

private slots:
	void on_WDM_bDirMakeCurrent_clicked();
	void on_WDM_bDirAdd_clicked();
	void on_WDM_bDirUp_clicked();
	void on_WDM_bDirDown_clicked();
	void on_WDM_bDirDeleteAll_clicked();
	void on_buttonBox_Cancel_clicked() {
		QDialog::close();
	}

private:
	Ui::WoWDirManager *ui_WoWDirManager;
};

QString WoWDirGroupName(st_WoWDir);
st_WoWDir ScanWoWDir(QDir, int, int);

#endif
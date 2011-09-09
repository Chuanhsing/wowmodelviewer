#ifndef WOWDIRMANAGER_H
#define WOWDIRMANAGER_H

#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include "ui_WoWDirManager.h"
#include "classes.h"


class WoWDirManager : public QDialog
{
	Q_OBJECT

public:
    explicit WoWDirManager(QWidget *parent = 0);
	~WoWDirManager();

private slots:
	void on_WDM_bDirAdd_clicked();
	void on_buttonBox_Cancel_clicked() {
		QDialog::close();
	}

private:
	Ui::WoWDirManager *ui_WoWDirManager;
};

st_WoWDir ScanWoWDir(QDir, int, int);

#endif
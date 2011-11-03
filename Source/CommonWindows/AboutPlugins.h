#ifndef WMV_ABOUT_PLUGINS_WINDOW_H
#define WMV_ABOUT_PLUGINS_WINDOW_H

#include "../Engine/kernel.h"

#include "GeneratedFiles/ui_About_Plugins.h"

namespace Ui {
	class AboutPlugins;
}

namespace WMVEngine {

// About Window
class WindowAboutPlugins : public QDialog
{
	Q_OBJECT

public:
    WMV_ENGINE_API explicit WindowAboutPlugins(QWidget *parent = 0);
	WMV_ENGINE_API ~WindowAboutPlugins();
	WMV_ENGINE_API void init();

private:
	QListWidget *List;

	void SetupList();
	void UpdateList();

private slots:
	void on_closeButton_clicked() {
		QDialog::close();
	}

private:
	Ui::AboutPlugins *ui_AboutPlugins;
};

} // Namespace

#endif
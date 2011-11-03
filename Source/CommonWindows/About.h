#ifndef WMV_ABOUT_WINDOW_H
#define WMV_ABOUT_WINDOW_H

#include "../Engine/wmv_engine.h"

#include "GeneratedFiles/ui_About.h"

namespace Ui {
	class AboutWindow;
}

namespace WMVEngine {

// About Window
class WindowAbout : public QDialog
{
	Q_OBJECT

public:
    WMV_ENGINE_API explicit WindowAbout(QWidget *parent = 0);
	WMV_ENGINE_API ~WindowAbout();
private slots:
	void on_CloseButton_clicked() {
		QDialog::close();
	}

private:
	Ui::AboutWindow *ui_About;
};

} // Namespace

#endif
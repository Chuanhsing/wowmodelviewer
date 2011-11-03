#include <qdatetime.h>
#include "About.h"
#include "../Engine/version.h"

using namespace WMVEngine;
using namespace QsLogging;

// About Window
WindowAbout::WindowAbout(QWidget *parent) : QDialog(parent), ui_About(new Ui::AboutWindow)
{
	QLOG_INFO() << "Logging Test, About Window";

	// Setup Window
	ui_About->setupUi(this);

	// Set Various Variables
	ui_About->WMVName->setText(PROGRAMNAME);
	ui_About->VersionNumber->setText(ui_About->VersionNumber->text().arg(MAJORVERSION,BUILDVERSION));
	ui_About->Edition->setText(ui_About->Edition->text().arg(SYSTEMVERSION,DEBUGVERSION));
	ui_About->ContentCopyright->setText(ui_About->ContentCopyright->text().arg(QDate().currentDate().toString("yyyy")));
}
WindowAbout::~WindowAbout()
{
	delete ui_About;
}
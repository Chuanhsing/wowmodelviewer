#include <QProcess>
#include <qdir.h>
#include <qmessagebox.h>
#include "Interface_Cinema.h"
#include "../Engine/version.h"
#include "../Engine/lists.h"

using namespace WMVEngine;
using namespace QsLogging;

// Viewer Interface Window
Interface_Cinema::Interface_Cinema(QWidget *parent) : QMainWindow(parent), ui(new Ui::Interface_Cinema)
{

	QLOG_INFO() << "Interface Cinema test";

	// Setup the UI
    ui->setupUi(this);

	// Passed arguments: ProgramName (WMV), MajorVersion (v0.8.0.0), BuildVersionName (Tofu) SystemVersion (Windows 64-bit), DebugVersion (" Debug" or "")
	setWindowTitle(windowTitle().arg(PROGRAMNAME,MAJORVERSION,BUILDVERSIONNAME,SYSTEMVERSION,DEBUGVERSION));

	// Viewer Modes
	ViewerModeGroup = new QActionGroup(this);
	ViewerModeGroup->addAction(ui->actionMode_Viewer);
	ViewerModeGroup->addAction(ui->actionMode_Cinema);
	ui->actionMode_Viewer->setChecked(true);

}

Interface_Cinema::~Interface_Cinema() {
	delete ui;
}

void Interface_Cinema::init()
{
	QLOG_INFO() << "Cinema Init Test!";

	//QLOG_INFO() << "Current WoW Directory:" << sWMVSettings.value("CurrentWoWDir").toString();
	//QLOG_INFO() << "Current WoW Version:" << CurrentDir.Version;

	// Build Status Bars
	createStatusBar();
	
	// Process and Update our Menus
	UpdateViewerMenu();
}

void Interface_Cinema::createStatusBar()
{
	statusBar()->setAutoFillBackground(true);
	statusBar()->setSizeGripEnabled(false);
	statusBar()->showMessage(tr("Ready"));
}

void Interface_Cinema::updateStatusBar(QString newstatus)
{
	statusBar()->showMessage(newstatus);
}

void Interface_Cinema::UpdateViewerMenu(){
		QLOG_INFO() << "Finished Updating the Cinema Menu.";
}

// Close WMV from File Menu
void Interface_Cinema::on_actionExit_triggered()
{
	QLOG_INFO() << "Quitting application...";
    QApplication::quit();
}

// This, sadly will have to be OS-dependant...
void Interface_Cinema::on_actionView_Log_triggered()
{
	QProcess *openProgram = new QProcess();
	QString file(QDir().absoluteFilePath(LOGFILE_FILENAME));
	
	QMessageBox err;
	err.setIcon(QMessageBox::Critical);

	if (QFile(file).exists() == false){
		QLOG_ERROR() << "Amazingly, this log file was not found!";		// Yes, I know logging this is redunant and possibly stupid, but you never know...
		updateStatusBar(tr("Error: Log file not found. Please check for it's existance."));
		err.setWindowTitle(tr("Error: Log file not found!"));
		err.setText(tr("Could not find the %1 file.\n\nCheck your permissions, as you might be running WMV in a location that does not allow for the generation of files.").arg(LOGFILE_FILENAME));
		err.exec();
		return;
	}

#ifdef _WINDOWS
	// Windows will open the file in Notepad.
	openProgram->start("notepad.exe \""+file+"\"",QIODevice::ReadOnly);
	QLOG_INFO() << "Opening Notepad to load" << file;
#elif defined (_MAC)
	// Mac will open the file using TextEdit
	openProgram->start("/Applications/TextEdit.app/Contents/MacOS/TextEdit \""+file+"\"",QIODevice::ReadOnly);
	QLOG_INFO() << "Opening TextEdit to load" << file;
#elif defined (_LINUX)
	// Linux might require some more juggling, depending on several factors, some of which are defined in the constants.h file.
	prog = LINUX_TXT_VIEWER;
	openProgram->start(LINUX_TXT_VIEWER+" \""+file+"\"",QIODevice::ReadOnly);
	QLOG_INFO() << "Opening" << static_cast<const char*>(LINUX_TXT_VIEWER) << "to load" << file;
#endif

	if (!openProgram->waitForStarted())
	{
		QLOG_ERROR() << "An error occured while trying to open the log file.";
		updateStatusBar(tr("There was an error while trying to open your text viewer."));
		err.setWindowTitle(tr("Error opening Log file..."));
		err.setText(tr("Could not open the log file for an unknown reason.\n\nPlease navigate to %1 and open the file manually.").arg(file));
		err.exec();
		return;
	}
	QLOG_INFO() << "Successfully opened the log file.";
}

// Open About Window
void Interface_Cinema::on_actionAbout_triggered()
{
	Window_About.show();
}

// Open About Plugins Window
void Interface_Cinema::on_actionAbout_Plugins_triggered()
{
	Window_AboutPlugins.init();
	Window_AboutPlugins.show();
}
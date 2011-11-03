#include "AboutPlugins.h"

using namespace WMVEngine;
using namespace QsLogging;

// About Window
WindowAboutPlugins::WindowAboutPlugins(QWidget *parent) : QDialog(parent), ui_AboutPlugins(new Ui::AboutPlugins)
{
	QLOG_INFO() << "Logging Test, About Plugins Window";

	// Setup Window
	ui_AboutPlugins->setupUi(this);

	// Set Up List
	SetupList();
}
WindowAboutPlugins::~WindowAboutPlugins()
{
	delete ui_AboutPlugins;
}

void WindowAboutPlugins::init()
{
	// Update List
	UpdateList();
}

void WindowAboutPlugins::SetupList()
{
	// Copy settings to our local list
	List = ui_AboutPlugins->pluginList;
	
	// Change various settings to fit our needs
	
	// Re-Apply the settings to the visible list.
	ui_AboutPlugins->pluginList = List;
}

// Update the List with our items!
void WindowAboutPlugins::UpdateList()
{
	DisplayServer ds = TheKernel.getDisplayServer();
	ModelExporterServer mes = TheKernel.getModelExporterServer();
	VideoExporterServer ves = TheKernel.getVideoExporterServer();

	size_t totalcount = 0;
	totalcount += ds.getDisplayDriverCount();
	totalcount += mes.getExporterTypeCount();
	totalcount += ves.getExporterTypeCount();

	ui_AboutPlugins->pluginCountRenderers->setText(ui_AboutPlugins->pluginCountRenderers->text().arg(ds.getDisplayDriverCount()));
	ui_AboutPlugins->pluginCountModelExporters->setText(ui_AboutPlugins->pluginCountModelExporters->text().arg(mes.getExporterTypeCount()));
	ui_AboutPlugins->pluginCountVideoExporters->setText(ui_AboutPlugins->pluginCountVideoExporters->text().arg(ves.getExporterTypeCount()));

	if (totalcount == 0) {
		QLOG_WARN() << "No Plugins Found.";
		List->clear();
		return;
	}

	QLOG_INFO() << "Here is where we would fill in the objects...";

}
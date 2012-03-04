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
	QLOG_INFO() << "Updating Plugin List...";

	QLOG_INFO() << "Kernel Test Text:" << TheKernel.TestText;
	QLOG_INFO() << "WMV::Kernel Test Text:" << WMVEngine::TheKernel.TestText;

	DisplayServer ds = WMVEngine::TheKernel.getDisplayServer();
	ModelExporterServer mes = WMVEngine::TheKernel.getModelExporterServer();
	VideoExporterServer ves = WMVEngine::TheKernel.getVideoExporterServer();

	size_t totalcount = 0;
	int dc = ds.getDisplayDriverCount();
	int mec = mes.getExporterTypeCount();
	int vec = ves.getExporterTypeCount();
	totalcount += dc + mec + vec;
	
	QLOG_INFO() << "Display Driver Count:" << dc;
	QLOG_INFO() << "Model Exporter Count:" << mec;
	QLOG_INFO() << "Video Exporter Count:" << vec;
	QLOG_INFO() << "Total Plugin Count:" << totalcount;

	ui_AboutPlugins->pluginCountRenderers->setText(ui_AboutPlugins->pluginCountRenderers->text().arg(dc));
	ui_AboutPlugins->pluginCountModelExporters->setText(ui_AboutPlugins->pluginCountModelExporters->text().arg(mec));
	ui_AboutPlugins->pluginCountVideoExporters->setText(ui_AboutPlugins->pluginCountVideoExporters->text().arg(vec));

	if (totalcount == 0) {
		QLOG_WARN() << "No Plugins Found.";
		List->clear();
		return;
	}

	QLOG_INFO() << "Found Plugins!\nHere is where we would fill in the objects...";

}
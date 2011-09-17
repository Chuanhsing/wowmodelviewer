#include "exporters.h"

Exporters::Exporters()
{
	bool performSync = false;

	// Initialize Default Settings
	ExporterSettingInit();

	// Exporter Intializers
	Lightwave3D_Init();
	
	// Make a temp list of the defaults.
	QMap<QString,QVariant> temp = SettingsExList;

	// If the last Version Number does not equal the current version number...
	// Bascially, don't run this if the version is the same.
	if (sWMVExSettings.value("Version") != MajorBuildVersion){
		QLOG_INFO() << "Exporter Settings appear out of date. Checking and updating as needed.";
		// Go over each Default setting
		for (QMap<QString,QVariant>::Iterator it=temp.begin();it!=temp.end();++it){
			// If the User has a setting using the same name, copy it's value into our temp list.
			if (sWMVExSettings.contains(it.key()) == true){
				temp[it.key()] = sWMVExSettings.value(it.key());
				performSync = true;
			}
		}
	}
	
	// Update the User's Settings
	if (performSync == true){
		// Delete the current settings
		sWMVExSettings.clear();

		// Insert the temp's keys and values.
		for (QMap<QString,QVariant>::Iterator it=temp.begin();it!=temp.end();++it){
			sWMVExSettings.setValue(it.key(),it.value());
		}

		// Set the current version
		// If we don't do this, Version will never be updated!
		sWMVExSettings.setValue("Version",MajorBuildVersion);

		// Save our new settings
		sWMVExSettings.sync();
		QLOG_INFO() << "Finished updating Exporter Settings.";
	}
}

Exporters::~Exporters()
{

}

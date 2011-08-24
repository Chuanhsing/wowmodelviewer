#include <QFile>
#include "Settings_Main.h"

// Check to see if the Main settings exist, and if not, set them.
void CheckSettings_Main(){
	bool performSync = false;

	// Initialize the default Settings
	SettingsListInit();

	// Make a temp list of the defaults.
	QMap<QString,QVariant> temp = SettingsList;

	// Check the Setting's Version number against the Current Version number.
	if (sWMVSettings.contains("Version") == false){								// If we can't find the version number, then there must be no settings.
		performSync = true;
	}else if (sWMVSettings.value("Version") != MAJORVERSION+BUILDVERSION){		// Else, If the version number doesn't match...
		// Go over each Default setting
		for (QMap<QString,QVariant>::Iterator it=temp.begin();it!=temp.end();++it){

			// If the User has a setting using the same name, copy it's value into our temp list.
			if (sWMVSettings.contains(it.key()) == true){
				temp[it.key()] = sWMVSettings.value(it.key());
				performSync = true;
			}
		}
	}

	// Update the User's Settings
	if (performSync == true){
		// Delete the current settings
		sWMVSettings.clear();

		// Insert the temp's keys and values.
		for (QMap<QString,QVariant>::Iterator it=temp.begin();it!=temp.end();++it){
			sWMVSettings.setValue(it.key(),it.value());
		}

		// Set the current version. If we don't do this, Version will never be updated!
		sWMVSettings.setValue("Version",MAJORVERSION+BUILDVERSION);

		// Save our new settings
		sWMVSettings.sync();
	}
}
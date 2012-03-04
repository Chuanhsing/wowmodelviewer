#include "Settings.h"

using namespace WMVEngine;

QMap<QString,QVariant> WMVEngine::SettingsList;		// List of Default Settings
QMap<QString,st_WoWDir> WMVEngine::WoWDirList;

// Initalization of our default settings
void WMVEngine::SettingsListInit()
{
	WMVEngine::SettingsList = QMap<QString,QVariant>();

	// Groups are done with a Prefix, such as "GroupName/VarName". Settings without a prefix will be listed in the General Group.
	SettingsList.insert("Version",MajorBuildVersion);					// Used to compare the settings version against the program's version.
	SettingsList.insert("StartupInterfaceMode",INTERFACEMODE_VIEWER);	// Default Interface mode.
	SettingsList.insert("CurrentWoWDir","None");						// Sets the Current WoW Dirto a String. String will match the names in the WoWDirList.
}

// Check to see if the Main settings exist, and if not, set them.
void WMVEngine::CheckSettings_Main(){
	bool performSync = false;

	// Initialize the default Settings
	WMVEngine::SettingsListInit();
	
	// Make a temp list of the defaults.
	QMap<QString,QVariant> temp = WMVEngine::SettingsList;

	// Check the Setting's Version number against the Current Version number.
	if (sWMVSettings.contains("Version") == false){								// If we can't find the version number, then there must be no settings.
		//g_WMV->updateStatusBar("Could not find Settings. Applying defaults...");
		QLOG_WARN() << "Could not find Settings. Applying defaults...";
		performSync = true;
	}else if (sWMVSettings.value("Version") != MajorBuildVersion){		// If the version number doesn't match...
		//g_WMV->updateStatusBar("Settings appear out of date. Checking and updating as needed...");
		QLOG_WARN() << "Settings appear out of date. Checking and updating as needed...";
		// Go over each Default setting
		for (QMap<QString,QVariant>::Iterator it=temp.begin();it!=temp.end();++it){
			// If the User has a setting using the same name, copy it's value into our temp list.
			if (sWMVSettings.contains(it.key()) == true){
				temp[it.key()] = sWMVSettings.value(it.key());
				performSync = true;
			}
		}
	}else if (sWMVSettings.childKeys().count() != WMVEngine::SettingsList.count()){	// If the number of settings don't match...
		QLOG_WARN() << "Incorrect number of settings. Fixing the issue...";
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
		QLOG_INFO() << "Saving the new settings...";
		// Delete the current settings
		sWMVSettings.clear();

		// Insert the temp's keys and values.
		for (QMap<QString,QVariant>::Iterator it=temp.begin();it!=temp.end();++it){
			sWMVSettings.setValue(it.key(),it.value());
		}

		// Set the current version. If we don't do this, Version will never be updated!
		sWMVSettings.setValue("Version",MajorBuildVersion);

		// Save our new settings
		sWMVSettings.sync();
		//g_WMV->updateStatusBar("Finished updating Settings.");
		QLOG_INFO() << "Finished updating Settings.";
	}
}

// Process the sWoWDirs and dump them into the WoWDirList
void WMVEngine::ReadWoWDirList(){
	QLOG_INFO() << "Rebuilding WoWDirList...";
	/*
	WMVEngine::WoWDirList.clear();		// Remove all previous items from the list. Should only contain things in the current settings.
	QStringList glist(sWoWDirs.childGroups());

	for (size_t i=0;i<glist.count();i++)
	{
		QString g = glist.value((int)i);
		//QLOG_TRACE() << "glist:" << g;
		st_WoWDir dir;
		sWoWDirs.beginGroup(g);

		// Add the saved values to our local st_WoWDir
		dir.Name = sWoWDirs.value("Name").toString();
		dir.Directory = QDir(sWoWDirs.value("Directory").toString());
		dir.Version = sWoWDirs.value("Version").toInt();
		dir.Position = sWoWDirs.value("Position").toInt();
		dir.Locale = sWoWDirs.value("Locale").toInt();

		// Process saved MPQ list...
		QString mpqs = sWoWDirs.value("MPQList").toString();
		QStringList mpqlist = mpqs.split(";");
		for (size_t m=0;m<mpqlist.count();m++){
			dir.MPQList.push_back(mpqlist.value((int)m));
		}
		sWoWDirs.endGroup();
		*/
		// Debug Messages
		/*
		QLOG_TRACE() << "Name:" << dir.Name;
		QLOG_TRACE() << "Dir:" << dir.Directory.path();
		QLOG_TRACE() << "Version:" << dir.Version;
		QLOG_TRACE() << "Locale:" << LocaleList.value(dir.Locale);
		QLOG_TRACE() << "MPQ List:" << mpqlist;
		*/
	/*
		// Insert our st_WoWDir into our directory list.
		WMVEngine::WoWDirList.insert(g,dir);
		//QLOG_TRACE() << "Post-Insert list count:" << WoWDirList.count();
	}

	QLOG_INFO() << "Finished rebuilding WoWDirList. Final count:" << WMVEngine::WoWDirList.count();
	*/
}
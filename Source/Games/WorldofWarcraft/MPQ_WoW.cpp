/*

	World of Warcraft-specific MPQ functions

*/

//#include "Settings_Main.h"
//#include "globals.h"
#include "MPQ_WoW.h"
#include "enums_WoW.h"
#include "lists_WoW.h"

using namespace WMVEngine::MPQ;

// --== MPQ File List==--

// Retrieves a list of MPQ files, based on the specified WoWDir's version.
// Returns both a QList AND places the list inside the specified WoWDir.
QList<QString> MPQList_Get(st_WoWDir &WoWDir){
	QList<QString> MPQList;
	QString locale = LocaleList.value(WoWDir.Locale);
	QDir dir(WoWDir.Directory);
	QLOG_INFO() << "Getting MPQ List...";

	// Change to WoW Data Directory
	// If changing to the Data directory fails, or it doesn't exist, then fail/return an empty list.
	if (dir.cd("Data") == false){
		QLOG_ERROR() << "Could not open Data directory. Returning empty MPQList...";
		return MPQList;
	}

	// MPQ Lists
	if (WoWDir.Version <= WOW_VANILLA){
		QLOG_INFO() << "Adding Vanilla MPQs to the List...";
		// Program MPQs
		MPQList.push_back("base.MPQ");

		// Common MPQs
		MPQList.push_back("dbc.MPQ");
		MPQList.push_back("fonts.MPQ");
		MPQList.push_back("interface.MPQ");
		MPQList.push_back("misc.MPQ");
		MPQList.push_back("model.MPQ");
		MPQList.push_back("sound.MPQ");
		MPQList.push_back("speech.MPQ");
		MPQList.push_back("terrain.MPQ");
		MPQList.push_back("texture.MPQ");
		MPQList.push_back("wmo.MPQ");

		// Patch MPQs
		if (QFile(dir.absolutePath()+"/patch.MPQ").exists())
			MPQList.push_back("patch.MPQ");
		if (QFile(dir.absolutePath()+"/patch-2.MPQ").exists())
			MPQList.push_back("patch-2.MPQ");
	}else if (WoWDir.Version <= WOW_TBC){
		QLOG_INFO() << "Adding Burning Crusade MPQs to the List...";
		// Locale Directory
		QDir ldir(dir);
		ldir.cd(locale);
		QString sldir(ldir.dirName()+"/");

		// Program MPQs
		MPQList.push_back(sldir+"base-"+locale+".MPQ");
		MPQList.push_back(sldir+"backup-"+locale+".MPQ");

		// Common MPQs
		MPQList.push_back("common.MPQ");
		MPQList.push_back("expansion.MPQ");
		if (QFile(dir.absolutePath()+"/patch.MPQ").exists())
			MPQList.push_back("patch.MPQ");
		if (QFile(dir.absolutePath()+"/patch-2.MPQ").exists())
			MPQList.push_back("patch-2.MPQ");

		// Locale MPQs
		MPQList.push_back(sldir+"locale-"+locale+".MPQ");
		MPQList.push_back(sldir+"speech-"+locale+".MPQ");
		MPQList.push_back(sldir+"expansion-locale-"+locale+".MPQ");
		MPQList.push_back(sldir+"expansion-speech-"+locale+".MPQ");
		if (QFile(ldir.absolutePath()+"/patch-"+locale+".MPQ").exists())
			MPQList.push_back(sldir+"patch-"+locale+".MPQ");
		if (QFile(ldir.absolutePath()+"/patch-"+locale+"-2.MPQ").exists())
			MPQList.push_back(sldir+"patch-"+locale+"-2.MPQ");
	}else if (WoWDir.Version <= WOW_WOTLK){
		QLOG_INFO() << "Adding Wrath of the Lich King MPQs to the List...";
		// Locale Directory
		QDir ldir(dir);
		ldir.cd(locale);
		QString sldir(ldir.dirName()+"/");

		// Program MPQs
		MPQList.push_back(sldir+"base-"+locale+".MPQ");
		MPQList.push_back(sldir+"backup-"+locale+".MPQ");

		// Common MPQs
		MPQList.push_back("common.MPQ");
		if (QFile(dir.absolutePath()+"/common-2.MPQ").exists())
			MPQList.push_back("common-2.MPQ");
		MPQList.push_back("expansion.MPQ");
		MPQList.push_back("lichking.MPQ");
		if (QFile(dir.absolutePath()+"/patch.MPQ").exists())
			MPQList.push_back("patch.MPQ");
		if (QFile(dir.absolutePath()+"/patch-2.MPQ").exists())
			MPQList.push_back("patch-2.MPQ");
		if (QFile(dir.absolutePath()+"/patch-3.MPQ").exists())
			MPQList.push_back("patch-3.MPQ");

		// Locale MPQs
		MPQList.push_back(sldir+"locale-"+locale+".MPQ");
		MPQList.push_back(sldir+"speech-"+locale+".MPQ");
		MPQList.push_back(sldir+"expansion-locale-"+locale+".MPQ");
		MPQList.push_back(sldir+"expansion-speech-"+locale+".MPQ");
		MPQList.push_back(sldir+"lichking-locale-"+locale+".MPQ");
		MPQList.push_back(sldir+"lichking-speech-"+locale+".MPQ");
		if (QFile(ldir.absolutePath()+"/patch-"+locale+".MPQ").exists())
			MPQList.push_back(sldir+"patch-"+locale+".MPQ");
		if (QFile(ldir.absolutePath()+"/patch-"+locale+"-2.MPQ").exists())
			MPQList.push_back(sldir+"patch-"+locale+"-2.MPQ");
		if (QFile(ldir.absolutePath()+"/patch-"+locale+"-3.MPQ").exists())
			MPQList.push_back(sldir+"patch-"+locale+"-3.MPQ");
	}else if (WoWDir.Version <= WOW_CATACLYSM){
		QLOG_INFO() << "Adding Cataclysm MPQs to the List...";
		// NOTE: We are NOT including the OldWorld.MPQ files.
		// If you want the old world, use an older WoW.

		// Locale Directory
		QDir ldir(dir);
		ldir.cd(locale);
		QString sldir(ldir.dirName()+"/");
		// Cache Directory
		QDir cachedir(dir);
		cachedir.cd("Cache");
		QString scdir(cachedir.dirName()+"/");
		// Cache Locale Directory
		QDir cacheldir(cachedir);
		cacheldir.cd(locale);
		QString scldir(scdir+cacheldir.dirName()+"/");

		// Program MPQs
		// Base-SYSTEM.MPQ replaces the base-LOCALE.MPQ and backup-LOCALE.MPQ files, but not until after 4.0.x
		#if defined (_WINDOWS) || defined (_LINUX)
		if (QFile(dir.absolutePath()+"/base-Win.MPQ").exists())
			MPQList.push_back("base-Win.MPQ");
		#elif defined (_MAC)
		// Just guessing at this name
		if (QFile(dir.absolutePath()+"/base-Mac.MPQ").exists())
			MPQList.push_back("base-Mac.MPQ");
		#endif
		// Keep these because 4.0.x still uses these
		if (QFile(ldir.absolutePath()+"/base-"+locale+".MPQ").exists())
			MPQList.push_back(sldir+"base-"+locale+".MPQ");			
		if (QFile(ldir.absolutePath()+"/backup-"+locale+".MPQ").exists())
			MPQList.push_back(sldir+"backup-"+locale+".MPQ");

		// Common MPQs
		MPQList.push_back("art.MPQ");
		MPQList.push_back("sound.MPQ");
		MPQList.push_back("world.MPQ");
		if (QFile(dir.absolutePath()+"/world2.MPQ").exists())
			MPQList.push_back("world2.MPQ");
		QDir dir_u(dir);
		dir_u.setFilter(QDir::Files);
		dir_u.setNameFilters(QStringList("expansion?.MPQ"));
		dir_u.setSorting(QDir::Name | QDir::IgnoreCase);
		for (size_t i=0;i<dir_u.count();i++){
			MPQList.push_back(dir_u.entryList().value((int)i));
		}

		// Update Files
		dir_u = dir;
		dir_u.setFilter(QDir::Files);								// Filter out the directories
		dir_u.setNameFilters(QStringList("wow-update-?????.MPQ"));	// Filter out everything but the Update MPQ files
		dir_u.setSorting(QDir::Name | QDir::IgnoreCase);
		for (size_t i=0;i<dir_u.count();i++){
			MPQList.push_back(dir_u.entryList().value((int)i));
		}

		// Locale MPQs
		MPQList.push_back(sldir+"locale-"+locale+".MPQ");
		MPQList.push_back(sldir+"speech-"+locale+".MPQ");

		// Locale Expansion Files
		dir_u = ldir;
		dir_u.setFilter(QDir::Files);
		QStringList f;
		f << "expansion?-locale-????.MPQ" << "expansion?-speech-????.MPQ";
		dir_u.setNameFilters(f);
		dir_u.setSorting(QDir::Name | QDir::IgnoreCase);
		for (size_t i=0;i<dir_u.count();i++){
			MPQList.push_back(sldir+dir_u.entryList().value((int)i));
		}

		// Locale Update Files
		dir_u = ldir;
		dir_u.setFilter(QDir::Files);
		dir_u.setNameFilters(QStringList("wow-update-????-?????.MPQ"));
		dir_u.setSorting(QDir::Name | QDir::IgnoreCase);
		for (size_t i=0;i<dir_u.count();i++){
			MPQList.push_back(sldir+dir_u.entryList().value((int)i));
		}

		// Cache MPQs
		dir_u = cachedir;
		dir_u.setFilter(QDir::Files);
		f.clear();
		f << "patch-base*.MPQ" << "SoundCache-?.MPQ" << "SoundCache-patch*.MPQ";
		dir_u.setNameFilters(f);
		dir_u.setSorting(QDir::Name | QDir::IgnoreCase);
		for (size_t i=0;i<dir_u.count();i++){
			MPQList.push_back(scdir+dir_u.entryList().value((int)i));
		}

		// Cache Locale MPQs
		dir_u = cacheldir;
		dir_u.setFilter(QDir::Files);
		f.clear();
		f << "patch-????-?????.MPQ" << "SoundCache-????.MPQ" << "SoundCache-patch*.MPQ";
		dir_u.setNameFilters(f);
		dir_u.setSorting(QDir::Name | QDir::IgnoreCase);
		for (size_t i=0;i<dir_u.count();i++){
			MPQList.push_back(scldir+dir_u.entryList().value((int)i));
		}
	}else if (WoWDir.Version == WOW_PTR){
		QLOG_INFO() << "Adding PTR MPQs to the List...";
		// Copied from the latest Version of WoW

		// Locale Directory
		QDir ldir(dir);
		ldir.cd(locale);
		QString sldir(ldir.dirName()+"/");
		// Cache Directory
		QDir cachedir(dir);
		cachedir.cd("Cache");
		QString scdir(cachedir.dirName()+"/");
		// Cache Locale Directory
		QDir cacheldir(cachedir);
		cacheldir.cd(locale);
		QString scldir(scdir+cacheldir.dirName()+"/");

		// Program MPQs
		// Base-SYSTEM.MPQ replaces the base-LOCALE.MPQ and backup-LOCALE.MPQ files
		#if defined (_WINDOWS) || defined (_LINUX)
		MPQList.push_back("base-Win.MPQ");
		#elif defined (_MAC)
		MPQList.push_back("base-Mac.MPQ");	// Just guessing at this name
		#endif

		// Common MPQs
		MPQList.push_back("art.MPQ");
		MPQList.push_back("sound.MPQ");
		MPQList.push_back("world.MPQ");
		if (QFile(dir.absolutePath()+"/world2.MPQ").exists())
			MPQList.push_back("world2.MPQ");
		QDir dir_u(dir);
		dir_u.setFilter(QDir::Files);
		dir_u.setNameFilters(QStringList("expansion?.MPQ"));
		dir_u.setSorting(QDir::Name | QDir::IgnoreCase);
		for (size_t i=0;i<dir_u.count();i++){
			MPQList.push_back(dir_u.entryList().value((int)i));
		}

		// Update Files
		dir_u = dir;
		dir_u.setFilter(QDir::Files);								// Filter out the directories
		dir_u.setNameFilters(QStringList("wow-update-?????.MPQ"));	// Filter out everything but the Update MPQ files
		dir_u.setSorting(QDir::Name | QDir::IgnoreCase);
		for (size_t i=0;i<dir_u.count();i++){
			MPQList.push_back(dir_u.entryList().value((int)i));
		}

		// Locale MPQs
		MPQList.push_back(sldir+"locale-"+locale+".MPQ");
		MPQList.push_back(sldir+"speech-"+locale+".MPQ");

		// Locale Expansion Files
		dir_u = ldir;
		dir_u.setFilter(QDir::Files);
		QStringList f;
		f << "expansion?-locale-????.MPQ" << "expansion?-speech-????.MPQ";
		dir_u.setNameFilters(f);
		dir_u.setSorting(QDir::Name | QDir::IgnoreCase);
		for (size_t i=0;i<dir_u.count();i++){
			MPQList.push_back(sldir+dir_u.entryList().value((int)i));
		}

		// Locale Update Files
		dir_u = ldir;
		dir_u.setFilter(QDir::Files);
		dir_u.setNameFilters(QStringList("wow-update-????-?????.MPQ"));
		dir_u.setSorting(QDir::Name | QDir::IgnoreCase);
		for (size_t i=0;i<dir_u.count();i++){
			MPQList.push_back(sldir+dir_u.entryList().value((int)i));
		}

		// Cache MPQs
		dir_u = cachedir;
		dir_u.setFilter(QDir::Files);
		f.clear();
		f << "patch-base*.MPQ" << "SoundCache-?.MPQ" << "SoundCache-patch*.MPQ";
		dir_u.setNameFilters(f);
		dir_u.setSorting(QDir::Name | QDir::IgnoreCase);
		for (size_t i=0;i<dir_u.count();i++){
			MPQList.push_back(scdir+dir_u.entryList().value((int)i));
		}

		// Cache Locale MPQs
		dir_u = cacheldir;
		dir_u.setFilter(QDir::Files);
		f.clear();
		f << "patch-????-?????.MPQ" << "SoundCache-????.MPQ" << "SoundCache-patch*.MPQ";
		dir_u.setNameFilters(f);
		dir_u.setSorting(QDir::Name | QDir::IgnoreCase);
		for (size_t i=0;i<dir_u.count();i++){
			MPQList.push_back(scldir+dir_u.entryList().value((int)i));
		}
	}else if ((WoWDir.Version == WOW_BETA) || (WoWDir.Version == WOW_MOP)){
		QLOG_INFO() << "Adding Beta/Mists of Pandaria MPQs to the List...";
		// No clue about the current beta/Mists of Pandaria, so this is empty for now.
	}
	QLOG_TRACE() << "Final MPQ List:";
	for (size_t i=0;i<MPQList.count();i++){
		QLOG_TRACE() << "   " << static_cast<const char*>(MPQList.value((int)i).toUtf8());
	}

	WoWDir.MPQList = MPQList;

	return MPQList;
}
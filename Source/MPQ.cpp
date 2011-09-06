#include "MPQ.h"

typedef QMap<QString, HANDLE*> ArchiveSet;
static ArchiveSet LoadedArchives;

void MPQArchive::Close()
{
	// If the file wasn't properly loaded, then there's nothing to close.
	if (OK == false)
		return;

	// Close the Current Archive
	SFileCloseArchive(MPQArch);

	// Remove from the list of loaded Archive Files
	for (ArchiveSet::Iterator iter=LoadedArchives.begin(); iter!=LoadedArchives.end(); ++iter) {
		HANDLE thisMPQ = iter.value();
		if (thisMPQ == MPQArch){
			LoadedArchives.erase(iter);
			return;
		}
	}
}

// Retrieves a list of MPQ files, based on the specified WoWDir's version.
QList<QString> MPQList_Get(stWoWDir WoWDir){
	QList<QString> MPQList;
	QString locale = LocaleList.value(WoWDir.Version);
	QDir dir(WoWDir.Directory);

	// Change to WoW Data Directory
	// If changing to the Data directory fails, or it doesn't exist, then fail/return an empty list.
	if (dir.cd("Data") == false)
		return MPQList;

	// MPQ Lists
	if (WoWDir.Version <= WOW_VANILLA){
		MPQList.push_back("base.MPQ");
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
		MPQList.push_back("patch.MPQ");
		MPQList.push_back("patch-2.MPQ");
	}else if (WoWDir.Version <= WOW_TBC){
		// Locale Directory
		QDir ldir(dir);
		ldir.cd(locale);

		// Common MPQs
		MPQList.push_back("common.MPQ");
		MPQList.push_back("expansion.MPQ");
		MPQList.push_back("patch.MPQ");
		MPQList.push_back("patch-2.MPQ");

		// Locale MPQs
		MPQList.push_back(ldir.path()+"base-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"backup-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"locale-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"speech-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"expansion-locale-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"expansion-speech-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"patch-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"patch-"+locale+"-2.MPQ");
	}else if (WoWDir.Version <= WOW_WOTLK){
		// Locale Directory
		QDir ldir(dir);
		ldir.cd(locale);

		// Common MPQs
		MPQList.push_back("common.MPQ");
		MPQList.push_back("common-2.MPQ");
		MPQList.push_back("expansion.MPQ");
		MPQList.push_back("lichking.MPQ");
		MPQList.push_back("patch.MPQ");
		MPQList.push_back("patch-2.MPQ");
		MPQList.push_back("patch-3.MPQ");

		// Locale MPQs
		MPQList.push_back(ldir.path()+"base-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"backup-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"locale-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"speech-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"expansion-locale-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"expansion-speech-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"lichking-locale-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"lichking-speech-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"patch-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"patch-"+locale+"-2.MPQ");
		MPQList.push_back(ldir.path()+"patch-"+locale+"-3.MPQ");
	}else if (WoWDir.Version <= WOW_CATACLYSM){
		// NOTE: We are NOT including the OldWorld.MPQ files.
		// If you want the old world, use an older WoW.

		// Locale Directory
		QDir ldir(dir);
		ldir.cd(locale);
		// Cache Directory
		QDir cachedir(dir);
		cachedir.cd("Cache");
		// Cache Locale Directory
		QDir cacheldir(cachedir);
		cacheldir.cd(locale);

		// Common MPQs
		// Base-SYSTEM.MPQ replaces the base-LOCALE.MPQ and backup-LOCALE.MPQ files
		#if defined (_WINDOWS) || defined (_LINUX)
		MPQList.push_back("base-Win.MPQ");
		#elif defined (_MAC)
		MPQList.push_back("base-Mac.MPQ");	// Just guessing at this name
		#endif
		MPQList.push_back("art.MPQ");
		MPQList.push_back("sound.MPQ");
		MPQList.push_back("world.MPQ");
		MPQList.push_back("expansion1.MPQ");
		MPQList.push_back("expansion2.MPQ");
		MPQList.push_back("expansion3.MPQ");

		// Update Files
		QDir dir_u(dir);
		dir_u.setFilter(QDir::Files);								// Filter out the directories
		dir_u.setNameFilters(QStringList("wow-update-?????.MPQ"));	// Filter out everything but the Update MPQ files
		dir_u.setSorting(QDir::Name);
		for (size_t i=0;i<dir_u.count();i++){
			MPQList.push_back(dir_u.entryList().value((int)i));
		}

		// Locale MPQs
		MPQList.push_back(ldir.path()+"base-"+locale+".MPQ");		// Kept because 4.0.x still uses these
		MPQList.push_back(ldir.path()+"backup-"+locale+".MPQ");		// Kept because 4.0.x still uses these
		MPQList.push_back(ldir.path()+"locale-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"speech-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"expansion1-locale-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"expansion1-speech-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"expansion2-locale-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"expansion2-speech-"+locale+".MPQ");

		// Locale Update Files
		dir_u = ldir;
		dir_u.setFilter(QDir::Files);
		dir_u.setNameFilters(QStringList("wow-update-????-?????.MPQ"));
		dir_u.setSorting(QDir::Name);
		for (size_t i=0;i<dir_u.count();i++){
			MPQList.push_back(dir_u.entryList().value((int)i));
		}

		// Cache MPQs
		dir_u = cachedir;
		dir_u.setFilter(QDir::Files);
		QStringList f;
		f << "patch-base*.MPQ" << "SoundCache-?.MPQ" << "SoundCache-patch*.MPQ";
		dir_u.setNameFilters(f);
		dir_u.setSorting(QDir::Name);
		for (size_t i=0;i<dir_u.count();i++){
			MPQList.push_back(dir_u.entryList().value((int)i));
		}

		// Cache Locale MPQs
		dir_u = cacheldir;
		dir_u.setFilter(QDir::Files);
		f.clear();
		f << "patch-????-?????.MPQ" << "SoundCache-????.MPQ" << "SoundCache-patch*.MPQ";
		dir_u.setNameFilters(f);
		dir_u.setSorting(QDir::Name);
		for (size_t i=0;i<dir_u.count();i++){
			MPQList.push_back(dir_u.entryList().value((int)i));
		}
	}else if (WoWDir.Version == WOW_PTR){
		// Copied from the latest Version of WoW

		// Locale Directory
		QDir ldir(dir);
		ldir.cd(locale);
		// Cache Directory
		QDir cachedir(dir);
		cachedir.cd("Cache");
		// Cache Locale Directory
		QDir cacheldir(cachedir);
		cacheldir.cd(locale);

		// Common MPQs
		// Base-SYSTEM.MPQ replaces the base-LOCALE.MPQ and backup-LOCALE.MPQ files
		#if defined (_WINDOWS) || defined (_LINUX)
		MPQList.push_back("base-Win.MPQ");
		#elif defined (_MAC)
		MPQList.push_back("base-Mac.MPQ");	// Just guessing at this name
		#endif
		MPQList.push_back("art.MPQ");
		MPQList.push_back("sound.MPQ");
		MPQList.push_back("world.MPQ");
		MPQList.push_back("expansion1.MPQ");
		MPQList.push_back("expansion2.MPQ");
		MPQList.push_back("expansion3.MPQ");

		// Update Files
		QDir dir_u(dir);
		dir_u.setFilter(QDir::Files);								// Filter out the directories
		dir_u.setNameFilters(QStringList("wow-update-?????.MPQ"));	// Filter out everything but the Update MPQ files
		dir_u.setSorting(QDir::Name);
		for (size_t i=0;i<dir_u.count();i++){
			MPQList.push_back(dir_u.entryList().value((int)i));
		}

		// Locale MPQs
		MPQList.push_back(ldir.path()+"locale-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"speech-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"expansion1-locale-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"expansion1-speech-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"expansion2-locale-"+locale+".MPQ");
		MPQList.push_back(ldir.path()+"expansion2-speech-"+locale+".MPQ");

		// Locale Update Files
		dir_u = ldir;
		dir_u.setFilter(QDir::Files);
		dir_u.setNameFilters(QStringList("wow-update-????-?????.MPQ"));
		dir_u.setSorting(QDir::Name);
		for (size_t i=0;i<dir_u.count();i++){
			MPQList.push_back(dir_u.entryList().value((int)i));
		}

		// Cache MPQs
		dir_u = cachedir;
		dir_u.setFilter(QDir::Files);
		QStringList f;
		f << "patch-base*.MPQ" << "SoundCache-?.MPQ" << "SoundCache-patch*.MPQ";
		dir_u.setNameFilters(f);
		dir_u.setSorting(QDir::Name);
		for (size_t i=0;i<dir_u.count();i++){
			MPQList.push_back(dir_u.entryList().value((int)i));
		}

		// Cache Locale MPQs
		dir_u = cacheldir;
		dir_u.setFilter(QDir::Files);
		f.clear();
		f << "patch-????-?????.MPQ" << "SoundCache-????.MPQ" << "SoundCache-patch*.MPQ";
		dir_u.setNameFilters(f);
		dir_u.setSorting(QDir::Name);
		for (size_t i=0;i<dir_u.count();i++){
			MPQList.push_back(dir_u.entryList().value((int)i));
		}
	}else if (WoWDir.Version == WOW_BETA){
		// No clue about the current beta, so this is empty for now.
	}

	return MPQList;
}
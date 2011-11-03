#include "WoWDirectoryTools.h"
#include "lists.h"

using namespace WMVEngine;

// Build WoW Directory Group Name
// Group = drivepathtodirectory_Locale. Path is always Lower-case
QString WoWDirGroupName(st_WoWDir dir){
	return QString(QString(dir.Directory.absolutePath()).simplified().remove(QRegExp("[:\\./\\\\_-\\s]",Qt::CaseInsensitive)).toLower()+"_"+LocaleList.value(dir.Locale));
}
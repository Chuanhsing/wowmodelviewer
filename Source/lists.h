#ifndef WMV_LISTS_H
#define WMV_LISTS_H

// This is used for various lists and other static variables.

#include <QString>
#include <QList>
#include <QMap>
#include "version.h"
#include "constants.h"
#include "classes.h"

// Locale List
// Placement matches WoWLocale number
static QList<QString> LocaleList = QList<QString>()
	<< "enUS"		// American English
	<< "enGB"		// British English (Identical to enUS)
	<< "frFR"		// French
	<< "deDE"		// German
	<< "koKR"		// Korean
	<< "zhCN"		// Chinese Simplified
	<< "zhTW"		// Chinese Traditional
	<< "esES"		// Spanish Spanish
	<< "esMX"		// Mexican Spanish
	<< "ruRU"		// Russian
	<< "jaJP"		// Japanese
	<< "ptBR"		// Brazilian Portuguese
	<< "itIT"		// Italian	-- Found these two in the PTR Installer
	<< "plPL"		// Polish

	<< "None";		// Used for Error Detection. Should ALWAYS be last!

// Expansion Name List
static QList<QString> ExpansionNameList = QList<QString>()
	<< QObject::tr("Vanilla")
	<< QObject::tr("Burning Crusade")
	<< QObject::tr("Wrath of the Lich King")
	<< QObject::tr("Cataclysm")
	<< QObject::tr("Public Test Realm")
	<< QObject::tr("Beta Server");

typedef QMap<QString, HANDLE*> t_ArchiveSet;
static t_ArchiveSet LoadedArchives;

static QDir LastDir;

#endif